-- lua_client_test - addons/hideuidemo/libs/hideui.lua and the hideuidemo reference
-- addon under stock lua5.1, against a stand-in for _HideUI.dll, a stubbed
-- windower table (its from_shift_jis turns the Shift-JIS bytes 82 A0 into
-- UTF-8's E3 81 82, both a hiragana a, and nothing else) and a stubbed
-- config library. Run:
-- lua5.1 lua_client_test.lua <path to addons/hideuidemo>

local addon_dir = assert(arg[1], 'usage: lua5.1 lua_client_test.lua <addons/hideuidemo>')

local failures, checks = 0, 0
local function check(ok, what)
    checks = checks + 1
    if not ok then failures = failures + 1 end
    print((ok and 'PASS' or 'FAIL') .. '  ' .. what)
end

-- windower --------------------------------------------------------------------

local chat, handlers
local SJIS_A, UTF8_A = '\130\160', '\227\129\130'
local function reset_windower()
    chat, handlers = {}, {}
    windower = {
        add_to_chat = function(color, text) chat[#chat + 1] = {color = color, text = text} end,
        register_event = function(name, fn)
            handlers[name] = handlers[name] or {}
            table.insert(handlers[name], fn)
        end,
        from_shift_jis = function(text) return (text:gsub(SJIS_A, UTF8_A)) end,
    }
end
local function fire(name, ...)
    for _, fn in ipairs(handlers[name] or {}) do fn(...) end
end
local function chat_has(text)
    for _, line in ipairs(chat) do
        if line.text:find(text, 1, true) then return true end
    end
    return false
end

-- the native module -----------------------------------------------------------

-- status() as the engine gives it: the public fields, the rest under detail.
local function status_table(ok, err)
    local d = {pinned = ok, busy = false, queued = 0, events = 0, unmatched_open_keys = 0,
               open_stack_overflow = 0, drain_errors = 0, error = err,
               image = {path = 'C:\\addon\\libs\\_HideUI.dll', build = '0.7.2', abi = 5}}
    local s = {ok = ok, state = ok and 'installed' or 'failed', role = ok and 'resident' or 'none',
               engine = '0.7.2', handles = 1, dropped = 7, hidden = {}, blocked = {}, moved = {}, resized = {},
               macros_blocked = false, detail = d}
    if ok then
        d.daemon = {abi = 1, build = '1.0.0'}
        d.registry, d.rows, d.names, d.manager = '0x020022D8', 370, 370, '0x10621838'
        d.ui = {w = 1920, h = 1080}
        s.ui = {w = 1920, h = 1080}
        d.functions = {open_by_name = '0x1', ui_update = '0x2', staged_close = '0x3', show_path = '0x4',
                       set_position = '0x5', close_by_name = '0x6', compass_draw = '0x7', compass_global = '0x8'}
        d.resident = d.image
        s.hidden, s.moved = {'logwindo'}, {'equip'}
    end
    return s
end

local function run(text, color, escape) return {text = text, color = color, escape = escape or 0x1E} end
local choice = {
    name = 'query', id = 7,
    title = {text = 'Will you lend a hand?', segments = {run('Will you lend a hand?', 1)}, undecoded = 0},
    cancellable = true,
    options = {{text = 'Yes, gladly.', segments = {run('Yes, ', 1), run('gladly', 2), run('.', 1)},
                undecoded = 0, value = 1},
               {text = 'Not today.', segments = {run('Not today.', 1)}, undecoded = 0, value = 4},
               {text = '?Cancel?', segments = {run('?Cancel', 1), run('?', 0x79, 0x1F)}, undecoded = 2,
                value = 5}}}
local prompts = {
    query = choice,
    link5 = {name = 'link5', id = 2, slots = {{slot = 3, name = 'Shell One'}, {slot = 15, name = 'Shell Two'}}},
    arealist = {name = 'arealist', id = 4, mode = 0, level = 0,
                rows = {{text = 'Current Area', id = 4, kind = 'current_area', label = 'here'},
                        {text = 'Region 01', id = -2, kind = 'region', count = 1},
                        {text = 'Zone 231', id = 231, kind = 'zone', label = 'Z231'}}},
    passinpu = {name = 'passinpu', id = 1, max_length = 16},
    prtyjoin = {name = 'prtyjoin', id = 5, inviter = 'Zaldon', alliance = true},
}

-- A fresh table every call, as the engine's reply is.
local function copy(t)
    if type(t) ~= 'table' then return t end
    local out = {}
    for k, v in pairs(t) do out[k] = copy(v) end
    return out
end

-- The engine's names, as many as these tests use: windows, and the compass
-- and the target arrows, which are no windows but are listed, hidden and
-- blocked like one.
local windows = {'logwindo', 'logwin2', 'equip', 'menuwind', 'buff', 'ability', 'query', 'partywin', 'targetwi',
                 'subwindo', 'passinpu', 'prtyjoin', 'link5', 'arealist', 'delivery', 'post1', 'post2',
                 'playermo', 'persona', 'compass', 'targetarrow', 'rem4line', 'rem4li2', 'mogdoor', 'trade',
                 'handover'}

-- The settings as the engine lists them, one of each kind; color_say's
-- label in Shift-JIS.
local settings_list = {
    {name = 'music_volume', page = 'conf2win', kind = 'number', value = 100, default = 100, min = 0, max = 100},
    {name = 'footstep_effects', page = 'conf6win', kind = 'bool', value = true, default = true},
    {name = 'shadows', page = 'conf3win', kind = 'choice', value = 'normal', default = 'normal',
     choices = {'high', 'normal', 'off'}},
    {name = 'color_say', page = 'conftxtc', kind = 'color', value = {r = 128, g = 128, b = 128},
     default = {r = 128, g = 128, b = 128}, min = 0, max = 255, label = 'Say ' .. SJIS_A},
    {name = 'key1_move_up', page = 'k1assign', kind = 'key', value = 0x11, default = 0x11, min = 0, max = 255,
     label = 'Move Up'},
}

local native_state
local function make_native()
    native_state = {calls = {}, handles = {}, status = status_table(true), shutdown = {true}, prompts = prompts,
                    new_error = nil, results = {}, raises = {}, lists = 0}
    local native = {}
    local Handle = {}
    Handle.__index = Handle
    for _, verb in ipairs({'hide', 'unhide', 'block', 'unblock', 'move', 'move_group', 'reset', 'reset_group',
                           'reset_all', 'open', 'open_set', 'close', 'resize', 'answer', 'cancel', 'block_macros',
                           'unblock_macros', 'offer', 'withdraw', 'set_setting'}) do
        Handle[verb] = function(self, ...)
            table.insert(native_state.calls, {verb = verb, handle = self.name, args = {...}, n = select('#', ...)})
            if native_state.raises[verb] then
                error(native_state.raises[verb], 0)
            end
            local result = native_state.results[verb]
            if result then
                return unpack(result)
            end
            return true
        end
    end
    function Handle:poll()
        local queued = self.queue
        self.queue = {}
        local dropped = self.dropped_next or 0
        self.dropped_next = 0
        return queued, dropped
    end
    function Handle:release()
        self.released = true
        return true
    end
    function Handle:info(name)
        return {name = name, open = true, hidden = true, blocked = false, blockable = true, layer = 2,
                hidden_by = {'hideuidemo', 'other'}, blocked_by = {}, mine = {hidden = true, blocked = false},
                docked = false, covered = false, focused = true,
                rect = {x = 1, y = 2, w = 3, h = 4, right = 4, bottom = 6},
                default = {x = 1, y = 2, w = 3, h = 4, right = 4, bottom = 6}, origin = {x = 1, y = 2},
                cursor = name == 'query' and 2 or name == 'arealist' and 3 or 1,
                top = name == 'query' and 1 or name == 'arealist' and 2 or nil, items = 3,
                category = name == 'ability' and 2 or nil,
                category_name = name == 'ability' and 'pet_commands' or nil,
                elements = {{type = 'frame', x = 1, y = 2, w = 3, h = 4, right = 4, bottom = 6, text = ''},
                            {type = 'item', x = 1, y = 2, w = 3, h = 4, right = 4, bottom = 6,
                             text = native_state.item_text or 'Party'},
                            {type = 'cursor', x = 5, y = 6, text = ''}, {type = 'other', text = ''}},
                elements_truncated = false,
                resize = {min_rows = 1, max_rows = 6, holds = 'trigger'},
                memory = {position = {x = 5, y = 6, owner = 'hideuidemo', mine = true},
                          size = {w = 200, h = 100, owner = 'other', mine = false}},
                detail = {policy = 0x00020002, rows = {41}, address = '0x01000000', controller = '0x02000000',
                          state = 0, live_layer = 2, live_policy = 0x00020002, layout = 'driven'}}
    end
    function Handle:list()
        native_state.lists = native_state.lists + 1
        local all = {}
        for _, name in ipairs(windows) do
            all[name] = {name = name, layer = 2, open = false, blockable = name ~= 'query',
                         detail = {}}
        end
        all.logwindo.open, all.logwindo.hidden, all.logwindo.detail.dock = true, true, 'chat_log'
        all.equip.moved = true
        all.compass.open, all.compass.layer = true, nil
        all.targetarrow.layer = nil
        return all
    end
    function Handle:opened()
        return {'logwindo', 'equip'}
    end
    function Handle:focused()
        return native_state.focused == nil and 'equip' or native_state.focused
    end
    function Handle:remembered()
        return {equip = {position = {x = 5, y = 6, owner = 'hideuidemo', mine = true}},
                partywin = {position = {x = 1792, y = 830, group = 'chat_log', owner = 'hideuidemo', mine = true},
                            size = {rows = 3, owner = 'hideuidemo', mine = true}}}
    end
    function Handle:options(name)
        local o = native_state.prompts[name]
        if not o then
            return nil, name .. ' has no options'
        end
        return copy(o)
    end
    function Handle:pending()
        return copy(native_state.pending or {})
    end
    function Handle:rects()
        return {logwindo = {x = 16, y = 898, w = 1774, h = 166}, equip = {x = 300, y = 200, w = 200, h = 160}}
    end
    function Handle:macros()
        return copy(native_state.macros or {blocked = false, blocked_by = {}, mine = false})
    end
    function Handle:settings()
        if native_state.settings_fail then
            return nil, native_state.settings_fail
        end
        return copy(settings_list)
    end
    function Handle:setting(name)
        table.insert(native_state.calls, {verb = 'setting', handle = self.name, args = {name}, n = 1})
        for _, s in ipairs(settings_list) do
            if s.name == name then return copy(s.value) end
        end
        return nil, 'no such setting: ' .. tostring(name) .. ' (settings() lists them)'
    end
    -- The engine's own layout(), which hideui.lua no longer reads.
    function Handle:layout()
        native_state.native_layouts = (native_state.native_layouts or 0) + 1
        return {}
    end
    function Handle:groups()
        local g = {}
        for _, n in ipairs({'chat_log', 'party_list', 'target_window'}) do
            g[n] = {anchor = 'x', anchor_open = true, origin = {x = 16, y = 930}, members = {'a', 'b'}}
        end
        g.party_list.anchor_open, g.party_list.origin = false, nil
        g.party_list.waiting = {x = 1700, y = 800, owner = 'hideuidemo', mine = true}
        return g
    end
    function native.new(name)
        if type(name) ~= 'string' then
            error('bad argument #1 to \'new\' (string expected)')
        end
        if native_state.new_error then
            return nil, native_state.new_error
        end
        local h = setmetatable({name = name, queue = {}}, Handle)
        table.insert(native_state.handles, h)
        return h
    end
    function native.status()
        return copy(native_state.status)
    end
    function native.version()
        return 'hideui 0.7.2'
    end
    function native.shutdown()
        native_state.shutdown_called = (native_state.shutdown_called or 0) + 1
        return unpack(native_state.shutdown)
    end
    return native
end

local loaded_dll, loaded_symbol
package.loadlib = function(path, symbol)
    loaded_dll, loaded_symbol = path, symbol
    return function() return make_native() end
end

local function last_call(verb)
    for i = #native_state.calls, 1, -1 do
        if not verb or native_state.calls[i].verb == verb then return native_state.calls[i] end
    end
end

-- the client --------------------------------------------------------------------

reset_windower()
local hideui = dofile(addon_dir .. '/libs/hideui.lua')

check(loaded_dll == addon_dir .. '/libs/_HideUI.dll' and loaded_symbol == 'luaopen__HideUI',
    'loads _HideUI.dll from beside itself through luaopen__HideUI')
check(handlers.prerender and handlers.unload, 'registers prerender and unload')
check(hideui.version() == 'hideui 0.7.2', 'version is the engine\'s')

local ui = hideui.new('first')
check(ui and ui.name == 'first' and #chat == 0, 'new: a handle, nothing printed')
local ok, err = pcall(hideui.new, 42)
check(not ok and tostring(err):find('name must be a string', 1, true), 'new(42) raises: ' .. tostring(err))
ok, err = pcall(hideui.new)
local unnamed_raises = not ok and tostring(err):find('_addon.name', 1, true)
_addon = {name = 'from_addon'}
local named = hideui.new()
_addon = nil
check(unnamed_raises and named and named.name == 'from_addon' and native_state.handles[#native_state.handles].name
        == 'from_addon',
    'new() with no name takes _addon.name, and raises only with neither: ' .. tostring(err))
named:release()
ok, err = pcall(hideui.debug, 'yes')
check(not ok and tostring(err):find('true or false', 1, true), 'debug(\'yes\') raises')

check(ui:move('equip', 300, 200) == true, 'verbs return what the engine returns')
local call = last_call()
check(call.verb == 'move' and call.args[1] == 'equip' and call.args[2] == 300 and call.args[3] == 200,
    'verbs forward their arguments unchanged')
native_state.results.answer = {nil, 'a reply to query is already queued'}
local r, why = ui:answer('query', 1)
check(r == nil and why == 'a reply to query is already queued', 'a refusal comes back as nil and the reason')
native_state.results.answer = nil
local read_choice = ui:options('query')
check(read_choice.name == 'query' and read_choice.id == 7 and read_choice.title.text == choice.title.text
        and #read_choice.options == 3 and ui:focused() == 'equip' and #ui:opened() == 2,
    'reads return what the engine returns')
native_state.focused = false
local focus, focus_why = ui:focused()
native_state.focused = nil
check(focus == false and focus_why == nil, 'focused(): false, and no reason, when nothing has focus')

-- misuse raises at the addon's line, checked by hideui.lua itself. Each
-- call notes its own line in `at` first, on the same line.
local at
local function here() at = debug.getinfo(2, 'l').currentline end
local function raised_here(fn)
    at = nil
    local ok2, err2 = pcall(fn)
    return not ok2 and at ~= nil and tostring(err2):find('lua_client_test.lua:' .. at .. ': ', 1, true) ~= nil,
        tostring(err2)
end
local misuse = {
    {'hide(name): name must be a string', function() here(); ui:hide(42) end},
    {'move(name, x, y): x and y must be numbers', function() here(); ui:move('equip', 'x', 2) end},
    {'move(name, x, y): x and y must be numbers', function() here(); ui:move('equip', 1) end},
    {'move_group(group, x, y): group must be a string', function() here(); ui:move_group(nil, 1, 2) end},
    {'resize(name, rows) or resize(name, w, h): rows, w and h must be numbers', function() here(); ui:resize('partywin', 'ptw3') end},
    {'resize(name, rows) or resize(name, w, h): rows, w and h must be numbers', function() here(); ui:resize('equip', 3, 'x') end},
    {'info(name): name must be a string', function() here(); ui:info() end},
    {'options(name): name must be a string', function() here(); ui:options(false) end},
    {'answer(name, value, id): id must be the number options(name) gave', function() here(); ui:answer('query', 1, '7') end},
    {'cancel(name, id): id must be the number options(name) gave', function() here(); ui:cancel('query', {}) end},
    {'reset(name[, aspect]): name must be a string', function() here(); ui:reset(3) end},
    {"reset(name, aspect): aspect must be 'position' or 'size'", function() here(); ui:reset('equip', 'all') end},
    {'reset_group(group): group must be a string', function() here(); ui:reset_group(1) end},
    {'the prompt must be the table options() or pending() returned', function() here(); ui:answer({id = 7}, 1) end},
    {'a prompt table carries its own id', function() here(); ui:cancel(read_choice, 7) end},
    {'name must be a string, or the table options() or pending() returned',
     function() here(); ui:cancel(false) end},
    {'open(name[, argument]): field row must be a finite number',
     function() here(); ui:open('macro', {palette = 'alt', row = 1 / 0}) end},
    {'open_set(set[, argument]): the argument must be a non-empty string without = or ;',
     function() here(); ui:open_set('region', 'a=b') end},
    {'setting(name): name must be a string', function() here(); ui:setting(42) end},
    {'set_setting(name, value): name must be a string', function() here(); ui:set_setting(nil, 1) end},
    {'use ui:set_setting(...)', function() here(); ui.set_setting('music_volume', 1) end},
    {'use ui:settings(...)', function() here(); ui.settings() end},
    {'use ui:hide(...)', function() here(); ui.hide('logwindo') end},
    {'use ui:layout(...)', function() here(); ui.layout() end},
    {'use ui:on(...)', function() here(); ui.on('opened', function() end) end},
}
local misplaced = {}
for _, m in ipairs(misuse) do
    local at, text = raised_here(m[2])
    if not at or not text:find(m[1], 1, true) then misplaced[#misplaced + 1] = m[1] .. ' / ' .. text end
end
check(#misplaced == 0 and #native_state.calls == 2,
    'misuse raises before reaching the engine, at the addon\'s line: a wrong or missing argument, and a call'
        .. ' with a dot for a colon ("use ui:hide(...)")' .. (misplaced[1] and (': ' .. misplaced[1]) or ''))
native_state.raises.answer = "answer('query', value) takes the option's value, a number"
local at_line, engine_text = raised_here(function() here(); ui:answer('query', 'yes') end)
native_state.raises.answer = nil
check(at_line and engine_text:find("takes the option's value", 1, true),
    'misuse only the engine sees (an answer of the wrong type for its window) is raised again at the addon\'s'
        .. ' line: ' .. engine_text)

-- settings(), setting() and set_setting() through the client: each label
-- as UTF-8 with its bytes beside it, the rest as the engine gives it; the
-- name and the value handed on as they are, a color's table among them.
local listed = ui:settings()
local say = listed and listed[4] or {}
check(#listed == 5 and say.name == 'color_say' and say.label == 'Say ' .. UTF8_A and say.label_raw == 'Say ' .. SJIS_A
        and listed[1].label == nil and listed[1].label_raw == nil and listed[5].label == 'Move Up'
        and listed[5].label_raw == 'Move Up' and say.value.r == 128 and listed[3].choices[1] == 'high',
    'settings(): the engine\'s list, each label as UTF-8 and its bytes as label_raw')
check(ui:setting('shadows') == 'normal' and last_call('setting').args[1] == 'shadows',
    'setting(name): the engine\'s value for the name')
local color = {r = 1, g = 2, b = 3}
local set_ok = ui:set_setting('color_say', color)
call = last_call('set_setting')
check(set_ok == true and call.args[1] == 'color_say' and call.args[2] == color and call.n == 2,
    'set_setting(name, value): the name and the value to the engine as given, a table as it is')
native_state.results.set_setting = {nil, 'music_volume takes a whole number from 0 to 100'}
local set_r, set_why = ui:set_setting('music_volume', 101)
native_state.results.set_setting = nil
check(set_r == nil and set_why == 'music_volume takes a whole number from 0 to 100',
    'set_setting: a refusal comes back as nil and the reason')
native_state.raises.set_setting = "set_setting('shadows', value): shadows takes one of its choices' names, a string"
local set_line, set_text = raised_here(function() here(); ui:set_setting('shadows', 2) end)
native_state.raises.set_setting = nil
check(set_line and set_text:find("shadows takes one of its choices' names", 1, true),
    'a set_setting of the wrong type, which only the engine sees, is raised again at the addon\'s line: ' .. set_text)

ok, err = pcall(ui.on, ui, 'show', function() end)
check(not ok and tostring(err):find('no such event: show', 1, true), 'on: an unknown event raises: ' .. tostring(err))
ok, err = pcall(ui.on, ui, 'opened', 42)
check(not ok and tostring(err):find('needs a function', 1, true), 'on: a non-function raises')
ok, err = pcall(ui.on, ui, 'opened', 7, function() end)
check(not ok and tostring(err):find('window name', 1, true), 'on: a name that is no string raises')
local listed_before = native_state.lists
ok, err = pcall(ui.on, ui, 'opened', 'nosuch', function() end)
local ok2, err2 = pcall(ui.on, ui, 'covered', 'Chat_Log', function() end)
local ok3 = pcall(ui.on, ui, 'opened', 'EQUIP', function() end)
ui:off('opened', 'equip')
check(not ok and tostring(err):find('no such window: nosuch', 1, true) and not ok2
        and tostring(err2):find('no such window: Chat_Log', 1, true) and ok3 and native_state.lists == listed_before + 1,
    'on: an unknown window name raises, a group\'s too, a window\'s in any case does not; the names are read'
        .. ' from the engine once: ' .. tostring(err))
ok, err = pcall(ui.on, ui, 'pending', 'prtyjoin', function() end)
ok2, err2 = pcall(ui.off, ui, 'resync', 'equip')
check(not ok and tostring(err):find('pending events name no window', 1, true) and not ok2
        and tostring(err2):find('resync events name no window', 1, true),
    'on and off: pending and resync take no window name: ' .. tostring(err))
ok, err = pcall(ui.off, ui, 'opened', 'nosuch')
check(not ok and tostring(err):find('no such window: nosuch', 1, true), 'off: an unknown window name raises too')
do
    local calls_before = #native_state.calls
    local on_ok = pcall(ui.on, ui, 'opened', 'compass', function() end)
    ui:off('opened', 'compass')
    check(on_ok and ui:hide('compass') == true and last_call('hide').args[1] == 'compass'
            and #native_state.calls == calls_before + 1,
        'the compass is a name the engine lists: on(event, \'compass\', fn) passes, hide(\'compass\') reaches the engine')
    calls_before = #native_state.calls
    on_ok = pcall(ui.on, ui, 'closed', 'targetarrow', function() end)
    ui:off('closed', 'targetarrow')
    check(on_ok and ui:hide('targetarrow') == true and last_call('hide').args[1] == 'targetarrow'
            and #native_state.calls == calls_before + 1,
        'the target arrows are a name the engine lists: on(event, \'targetarrow\', fn) passes, hide(\'targetarrow\')'
            .. ' reaches the engine')
end

local seen = {}
local function record(event) seen[#seen + 1] = event.event .. ':' .. event.name end
local function boom() error('callback bug') end
local errors = {}
local function on_error(event) errors[#errors + 1] = event.verb .. ' ' .. event.name .. ': ' .. event.reason end
check(ui:on('opened', boom) == boom and ui:on('opened', record) == record and ui:on('closed', record) == record
        and ui:on('covered', 'equip', record) == record and ui:on('error', on_error) == on_error,
    'on returns the callback')

local first_native = native_state.handles[1]
local pendings, resyncs = {}, {}
ui:on('pending', function(event) pendings[#pendings + 1] = event.what .. '=' .. tostring(event.pending) end)
ui:on('resync', function(event) resyncs[#resyncs + 1] = event.dropped end)
first_native.queue = {{event = 'opened', name = 'equip'}, {event = 'closed', name = 'equip'},
                      {event = 'covered', name = 'menuwind'}, {event = 'covered', name = 'equip'},
                      {event = 'blocked', name = 'menuwind'},
                      {event = 'error', name = 'passinpu', verb = 'answer', reason = 'no text entry is pending'},
                      {event = 'pending', what = 'invite', pending = true},
                      {event = 'pending', what = 'post', pending = false},
                      {event = 'resync', dropped = 2}}
first_native.dropped_next = 2
fire('prerender')
check(table.concat(pendings, ' ') == 'invite=true post=false' and #resyncs == 1 and resyncs[1] == 2,
    'pending and resync events reach the callbacks registered for them: ' .. table.concat(pendings, ' '))
check(table.concat(seen, ' ') == 'opened:equip closed:equip covered:equip',
    'prerender: events reach their callbacks in order; on(event, name, fn) fires for that window only: '
        .. table.concat(seen, ' '))
check(#errors == 2 and errors[1] == 'callback equip: opened callback: ' .. errors[1]:match('opened callback: (.*)$')
        and errors[1]:find('callback bug', 1, true) and errors[2] == 'answer passinpu: no text entry is pending',
    'a failing callback is posted once as an error event (verb callback); the engine\'s error reaches the error'
        .. ' callback: ' .. table.concat(errors, ' | '))
check(#chat == 0, 'nothing printed with debug off')
check(ui.dropped == 2 and ui.callback_errors == 1, 'dropped events and failed callbacks are counted on the handle')

seen, errors = {}, {}
first_native.queue = {{event = 'opened', name = 'buff'}}
fire('prerender')
check(#seen == 1 and #errors == 0 and ui.callback_errors == 2,
    'the same callback failing again is counted, not posted again')
local s = ui:status()
local plain = hideui.status()
check(s.dropped == 2 and s.detail == nil and s.engine == '0.7.2' and plain.dropped == 7 and plain.detail == nil,
    'status() with debug off: this handle\'s dropped and the engine\'s, and no detail')
native_state.pending = {invite = {name = 'prtyjoin', inviter = 'Zaldon', alliance = false, id = 9},
                        post = {name = 'delivery', box = 'delivery', id = 3, detail = {state = 5}}}
local info, list, pending = ui:info('equip'), ui:list(), ui:pending()
check(info.detail == nil and info.hidden and list.logwindo.detail == nil and list.logwindo.hidden
        and pending.post.detail == nil and pending.post.box == 'delivery' and pending.invite.inviter == 'Zaldon',
    'info, list and pending with debug off: the public fields, no detail')
local ability = ui:info('ability')
check(ability.category == 2 and ability.category_name == 'pet_commands' and info.category == nil
        and info.category_name == nil,
    'info(ability).category and category_name pass through untouched; a window without them has neither')
hideui.debug(true)
s = ui:status()
check(s.dropped == 2 and s.detail.callback_errors == 2 and s.detail.functions.open_by_name == '0x1',
    'handle status() with debug on: this handle\'s callback errors and the engine\'s internals under detail')
s = hideui.status()
native_state.pending = {invite = {name = 'prtyjoin', inviter = 'Zaldon', alliance = false, id = 9},
                        post = {name = 'delivery', box = 'delivery', id = 3, detail = {state = 5}}}
info, list, pending = ui:info('equip'), ui:list(), ui:pending()
check(s.dropped == 7 and s.detail.callback_errors == 2 and info.detail.address == '0x01000000'
        and list.logwindo.detail.dock == 'chat_log' and pending.post.detail.state == 5,
    'with debug on: hideui.status() has this addon\'s callback errors, and info, list and pending their detail')
hideui.debug(false)
native_state.pending = nil

ui:off('opened', boom)
seen = {}
first_native.queue = {{event = 'opened', name = 'buff'}}
fire('prerender')
check(#seen == 1, 'off(event, fn) removes one callback')
ui:off('covered', 'equip')
seen = {}
first_native.queue = {{event = 'covered', name = 'equip'}}
fire('prerender')
check(#seen == 0, 'off(event, name) removes the callbacks for that window')
ui:off('opened')
seen = {}
first_native.queue = {{event = 'opened', name = 'buff'}}
fire('prerender')
check(#seen == 0, 'off(event) removes every callback for the event')
ok = pcall(ui.off, ui, 'shown')
check(not ok, 'off: an unknown event raises')

-- debug on: the library may print
hideui.debug(true)
ui:on('opened', boom)
first_native.queue = {{event = 'opened', name = 'buff'},
                      {event = 'error', name = 'query', verb = 'answer', reason = 'query is not open'}}
fire('prerender')
check(chat_has('opened callback failed: ') and chat_has('callback bug') and chat_has('answer query: query is not open'),
    'debug on: a failing callback and an error event are printed')
hideui.debug(false)
chat = {}

-- the new events: cursor, and blocked with its holders
do
    local got = {}
    ui:on('cursor', 'equip', function(e) got[#got + 1] = e.event .. ':' .. e.name .. ':' .. e.row end)
    ui:on('blocked', function(e) got[#got + 1] = e.event .. ':' .. table.concat(e.by, '+') .. ':' .. tostring(e.mine) end)
    first_native.queue = {{event = 'cursor', name = 'equip', row = 3}, {event = 'cursor', name = 'buff', row = 1},
                          {event = 'blocked', name = 'menuwind', by = {'first', 'other'}, mine = true}}
    fire('prerender')
    local bad, bad_why = pcall(ui.on, ui, 'cursor', 'nosuch', function() end)
    check(table.concat(got, ' ') == 'cursor:equip:3 blocked:first+other:true' and not bad
            and tostring(bad_why):find('no such window: nosuch', 1, true),
        'cursor events reach on(\'cursor\', name, fn) for their window, with the row; blocked carries by and mine: '
            .. table.concat(got, ' '))
    ui:off('cursor')
    ui:off('blocked')
end

-- rects
do
    local r = ui:rects()
    check(r.logwindo.x == 16 and r.logwindo.y == 898 and r.logwindo.w == 1774 and r.logwindo.h == 166 and r.equip,
        'rects(): every open window\'s frame, keyed by name')
end

-- the macro keys: two verbs of no window, and a read
do
    native_state.calls = {}
    native_state.macros = {blocked = true, blocked_by = {'first', 'other'}, mine = true}
    native_state.status.macros_blocked = true
    local m = ui:macros()
    local s = hideui.status()
    local blocked = ui:block_macros()
    local unblocked = ui:unblock_macros()
    local c1, c2 = native_state.calls[1] or {}, native_state.calls[2] or {}
    check(blocked == true and unblocked == true and #native_state.calls == 2 and c1.verb == 'block_macros' and c1.n == 0
            and c2.verb == 'unblock_macros' and c2.n == 0,
        'block_macros and unblock_macros reach the engine with no arguments and return what it returns')
    check(m.blocked == true and #m.blocked_by == 2 and m.blocked_by[1] == 'first' and m.mine == true
            and s.macros_blocked == true and s.detail == nil,
        'macros() and status().macros_blocked return what the engine returns')
    native_state.results.block_macros = {nil, 'block_macros needs hideui 0.9.0 or newer; the resident copy is X'}
    local r, why = ui:block_macros()
    native_state.results.block_macros = nil
    local dot_ok, dot_err = pcall(ui.block_macros)
    local dot2_ok, dot2_err = pcall(ui.macros)
    check(r == nil and why == 'block_macros needs hideui 0.9.0 or newer; the resident copy is X' and not dot_ok
            and tostring(dot_err):find('use ui:block_macros(...)', 1, true) and not dot2_ok
            and tostring(dot2_err):find('use ui:macros(...)', 1, true),
        'a refusal of block_macros comes back as nil and the reason; a dot for a colon raises: ' .. tostring(why))
    native_state.macros, native_state.status.macros_blocked = nil, false
end

-- open and open_set with an argument: it reaches the engine only when
-- given, a string as it is, a whole number as its digits, a table as its
-- fields key=value joined by ';' in key order; misuse raises at the addon's
-- line
do
    native_state.calls = {}
    local c1 = ui:open('ability', 'pet_commands')
    local c2 = ui:open('equip')
    local c3 = ui:open_set('equipment')
    local c4 = ui:open_set('magic', '6')
    local c5 = ui:open_set('synthesis', 12)
    local c6 = ui:open('ability', 2)
    local c7 = ui:open('iteminfo', {slot = 5, bag = 'inventory'})
    local c8 = ui:open('casttime', {style = 'countdown', seconds = 3})
    local c9 = ui:open_set('region', 'colonization')
    local c10 = ui:open('casttime', 2.5)
    local c11 = ui:open('eventtim', {seconds = 1 / 60})
    local calls = native_state.calls
    check(c1 == true and c2 == true and c3 == true and c4 == true and c5 == true and c6 == true and c7 == true
            and c8 == true and c9 == true and c10 == true and c11 == true and #calls == 11
            and calls[10].args[2] == '2.5' and calls[11].args[2] == 'seconds=0.016666666666666666'
            and calls[1].verb == 'open' and calls[1].n == 2 and calls[1].args[1] == 'ability'
            and calls[1].args[2] == 'pet_commands' and calls[2].verb == 'open' and calls[2].n == 1
            and calls[3].verb == 'open_set' and calls[3].n == 1 and calls[3].args[1] == 'equipment'
            and calls[4].verb == 'open_set' and calls[4].n == 2 and calls[4].args[2] == '6'
            and calls[5].verb == 'open_set' and calls[5].n == 2 and calls[5].args[2] == '12'
            and calls[6].args[2] == '2' and calls[7].args[2] == 'bag=inventory;slot=5'
            and calls[8].args[2] == 'seconds=3;style=countdown' and calls[9].args[2] == 'colonization',
        'open and open_set pass an argument to the engine only when given: a string as it is, a whole number as'
            .. ' its digits, any other number in decimal to 17 places, a table as key=value pairs in key order: '
            .. tostring(calls[11] and calls[11].args[2]))
    local ok, err = pcall(ui.open, ui, 'ability', true)
    local ok2, err2 = pcall(ui.open_set, ui, 7)
    local ok3, err3 = pcall(ui.open_set, 'equipment')
    local ok4, err4 = pcall(ui.open_set, ui, 'synthesis', 0 / 0)
    local ok5, err5 = pcall(ui.open, ui, 'macro', {palette = 'alt', row = math.huge})
    local ok6, err6 = pcall(ui.open, ui, 'macro', {'alt', 3})
    local ok7, err7 = pcall(ui.open, ui, 'macro', {})
    local ok8, err8 = pcall(ui.open, ui, 'mogpost', 'mode=nomad')
    local ok9, err9 = pcall(ui.open, ui, 'iteminfo', {bag = {}, slot = 1})
    check(not ok and tostring(err):find('open(name[, argument]): the argument must be a string or a number', 1,
                true)
            and not ok2 and tostring(err2):find('open_set(set[, argument]): set must be a string', 1, true)
            and not ok3 and tostring(err3):find('use ui:open_set(...)', 1, true)
            and not ok4 and tostring(err4):find('open_set(set[, argument]): the argument must be a finite number', 1,
                true)
            and not ok5 and tostring(err5):find('field row must be a finite number', 1, true)
            and not ok6 and tostring(err6):find('a table argument takes named fields only', 1, true)
            and not ok7 and tostring(err7):find('a table argument needs a field', 1, true)
            and not ok8 and tostring(err8):find('must be a non-empty string without = or ;', 1, true)
            and not ok9 and tostring(err9):find('field bag must be a string or a number', 1, true),
        'an argument that is no string, finite number or table of named fields, a string with = or ;, a set that'
            .. ' is no string, or a dot for a colon, raises at the addon\'s line: ' .. tostring(err))
    native_state.results.open_set = {nil, 'persona is blocked; the equipment set opens it'}
    local r, why = ui:open_set('equipment')
    native_state.results.open_set = nil
    check(r == nil and why == 'persona is blocked; the equipment set opens it',
        'the engine\'s refusal of open_set comes back as nil and the reason')
end

-- one list of ability: block and unblock with a category, on and off with a
-- category filter, and the category fields on ability's events
do
    native_state.calls = {}
    local c1 = ui:block('ability', 'pet_commands')
    local c2 = ui:unblock('ability', '2')
    local c3 = ui:block('menuwind')
    local c4 = ui:unblock('menuwind')
    local calls = native_state.calls
    check(c1 == true and c2 == true and c3 == true and c4 == true and #calls == 4
            and calls[1].verb == 'block' and calls[1].n == 2 and calls[1].args[1] == 'ability'
            and calls[1].args[2] == 'pet_commands' and calls[2].verb == 'unblock' and calls[2].n == 2
            and calls[2].args[2] == '2' and calls[3].n == 1 and calls[4].n == 1,
        'block and unblock pass a category to the engine only when given, as the string it is')
    local ok, err = pcall(ui.block, ui, 'ability', 2)
    local ok2, err2 = pcall(ui.unblock, ui, 'ability', {})
    check(not ok and tostring(err):find('block(name[, category]): category must be a string', 1, true) and not ok2
            and tostring(err2):find('unblock(name[, category]): category must be a string', 1, true),
        'a category that is no string raises at the addon\'s line: ' .. tostring(err))
    native_state.results.block = {nil, 'only ability has categories'}
    local r, why = ui:block('equip', 'pet_commands')
    native_state.results.block = nil
    check(r == nil and why == 'only ability has categories', 'the engine\'s refusal comes back as nil and the reason')

    local got = {}
    local pets = ui:on('opened', 'ability', 'pet_commands', function(e) got[#got + 1] = 'pets:' .. tostring(e.category) end)
    local two = ui:on('opened', 'ability', '2', function(e) got[#got + 1] = 'two:' .. tostring(e.category_name) end)
    ui:on('closed', 'ability', 'job_traits', function(e) got[#got + 1] = 'traits:' .. tostring(e.category) end)
    ui:on('blocked', 'ability', '20', function(e)
        got[#got + 1] = 'twenty:' .. tostring(e.category_name) .. ':' .. table.concat(e.by, '+') .. ':' .. tostring(e.mine)
    end)
    ui:on('opened', 'ability', function(e) got[#got + 1] = 'any:' .. tostring(e.category) end)
    first_native.queue = {{event = 'opened', name = 'ability', category = 2, category_name = 'pet_commands'},
                          {event = 'opened', name = 'ability', category = 1, category_name = 'job_abilities'},
                          {event = 'opened', name = 'ability'},
                          {event = 'closed', name = 'ability', category = 4, category_name = 'job_traits'},
                          {event = 'blocked', name = 'ability', category = 20, category_name = 'weapon_skills',
                           by = {'first'}, mine = true},
                          {event = 'opened', name = 'equip'}}
    fire('prerender')
    check(table.concat(got, ' ') == 'pets:2 two:pet_commands any:2 any:1 any:nil traits:4 twenty:weapon_skills:first:true',
        'on(event, ability, category, fn) fires for that list alone, by name or number, and passes category and'
            .. ' category_name through; without a category, for every list: ' .. table.concat(got, ' '))
    ok, err = pcall(ui.on, ui, 'opened', 'equip', 'pet_commands', function() end)
    ok2, err2 = pcall(ui.on, ui, 'opened', 'ability', 2, function() end)
    local ok3, err3 = pcall(ui.off, ui, 'closed', 'equip', 'job_traits')
    local ok4, err4 = pcall(ui.on, ui, 'opened', 'ability', 'pet_commands', 42)
    check(not ok and tostring(err):find('category filters apply to ability only', 1, true) and not ok2
            and tostring(err2):find('on(event, name, category, fn): category must be a string', 1, true) and not ok3
            and tostring(err3):find('category filters apply to ability only', 1, true) and not ok4
            and tostring(err4):find('needs a function', 1, true),
        'a category filter on another window raises, on and off alike; a category that is no string, and a'
            .. ' missing function: ' .. tostring(err))
    ui:off('opened', 'ability', 'pet_commands')
    got = {}
    first_native.queue = {{event = 'opened', name = 'ability', category = 2, category_name = 'pet_commands'}}
    fire('prerender')
    local after_off = table.concat(got, ' ')
    ui:off('opened', 'ability', '2', pets)
    got = {}
    first_native.queue = {{event = 'opened', name = 'ability', category = 2, category_name = 'pet_commands'}}
    fire('prerender')
    local wrong_fn = table.concat(got, ' ')
    ui:off('opened', 'ability', '2', two)
    got = {}
    first_native.queue = {{event = 'opened', name = 'ability', category = 2, category_name = 'pet_commands'}}
    fire('prerender')
    local right_fn = table.concat(got, ' ')
    ui:off('opened')
    ui:off('closed')
    ui:off('blocked')
    check(after_off == 'two:pet_commands any:2' and wrong_fn == 'two:pet_commands any:2' and right_fn == 'any:2',
        'off(event, ability, category[, fn]) removes the callbacks for that list, or that one of them, alone: '
            .. after_off .. ' / ' .. wrong_fn .. ' / ' .. right_fn)
end

-- the game's text as UTF-8, its bytes beside it as raw; segments' colors named
do
    native_state.prompts = copy(prompts)
    native_state.prompts.query.title = {text = 'Teleport ' .. SJIS_A,
        segments = {run('Teleport ', 1), run(SJIS_A, 2), run('?', 0x79, 0x1F), run('x', 5)}, undecoded = 1}
    native_state.prompts.link5.slots[1].name = 'Shell ' .. SJIS_A
    native_state.prompts.prtyjoin.inviter = 'Zal' .. SJIS_A
    native_state.prompts.arealist.rows[1].text = 'Area ' .. SJIS_A
    native_state.prompts.arealist.rows[1].label = SJIS_A
    local q = ui:options('query')
    local seg = q.title.segments
    check(q.title.text == 'Teleport ' .. UTF8_A and q.title.raw == 'Teleport ' .. SJIS_A and seg[2].text == UTF8_A
            and seg[2].raw == SJIS_A and seg[1].text == 'Teleport ' and seg[1].raw == 'Teleport ',
        'options(query): the title and each segment as UTF-8, the Shift-JIS bytes as raw beside each')
    check(seg[1].color == 'default' and seg[2].color == 'green' and seg[3].color == 'color377'
            and seg[4].color == 'color5' and seg[1].escape == nil and seg[3].escape == nil
            and q.options[1].segments[2].color == 'green' and q.options[1].text == 'Yes, gladly.'
            and q.options[1].raw == 'Yes, gladly.',
        'segments: 1E 01 default, 1E 02 green, any other as color<n> (1F nn as color<0x100 + nn>), no escape: '
            .. tostring(seg[3].color) .. ' ' .. tostring(seg[4].color))
    local l5, pj = ui:options('link5'), ui:options('prtyjoin')
    check(l5.slots[1].name == 'Shell ' .. UTF8_A and l5.slots[1].raw == 'Shell ' .. SJIS_A and l5.slots[2].name
            == 'Shell Two' and pj.inviter == 'Zal' .. UTF8_A and pj.raw == 'Zal' .. SJIS_A and pj.name == 'prtyjoin',
        'options: link5\'s names and prtyjoin\'s inviter as UTF-8 with raw beside them; the prompt keeps its name')
    local area = ui:options('arealist')
    local first, region = area.rows[1], area.rows[2]
    check(first.text == 'Area ' .. UTF8_A and first.raw == 'Area ' .. SJIS_A and first.label == UTF8_A
            and first.label_raw == SJIS_A and region.text == 'Region 01' and region.raw == 'Region 01'
            and region.count == 1 and region.label == nil and region.label_raw == nil,
        'options(arealist): each row\'s text and label as UTF-8, their bytes in raw and label_raw; a region\'s count'
            .. ' as it was')
    native_state.pending = {invite = {name = 'prtyjoin', inviter = 'Zal' .. SJIS_A, alliance = false, id = 9}}
    native_state.item_text = 'Item ' .. SJIS_A
    local pend, inf = ui:pending(), ui:info('equip')
    check(pend.invite.inviter == 'Zal' .. UTF8_A and pend.invite.raw == 'Zal' .. SJIS_A
            and inf.elements[2].text == 'Item ' .. UTF8_A and inf.elements[2].raw == 'Item ' .. SJIS_A
            and inf.elements[1].text == '' and inf.elements[1].raw == '',
        'pending().invite.inviter and info() element text as UTF-8, raw beside them')
    native_state.item_text, native_state.pending, native_state.prompts = nil, nil, prompts
end

-- the page event: on('page', fn) for every dialogue box, on('page', name,
-- fn) for one
do
    local pages, named = {}, {}
    local function on_page(event) pages[#pages + 1] = event.event .. ':' .. event.name end
    local function on_named(event) named[#named + 1] = event.name end
    ui:on('page', on_page)
    ui:on('page', 'rem4line', on_named)
    native_state.handles[1].queue = {{event = 'page', name = 'rem4line'}, {event = 'page', name = 'rem4li2'},
                                     {event = 'opened', name = 'rem4line'}}
    fire('prerender')
    ui:off('page', on_page)
    ui:off('page', 'rem4line', on_named)
    native_state.handles[1].queue = {{event = 'page', name = 'rem4line'}}
    fire('prerender')
    check(table.concat(pages, ' ') == 'page:rem4line page:rem4li2' and table.concat(named, ' ') == 'rem4line',
        'page events reach on(\'page\', fn), and on(\'page\', name, fn) for that box alone, until off: '
            .. table.concat(pages, ' ') .. '; ' .. table.concat(named, ' '))
end

-- replies take the prompt table options() or pending() gave
do
    native_state.calls = {}
    native_state.pending = {invite = {name = 'prtyjoin', inviter = 'Zaldon', alliance = false, id = 9},
                            post = {name = 'delivery', box = 'delivery', id = 3, detail = {state = 5}}}
    local q, p = ui:options('query'), ui:pending()
    local done = ui:answer(q, 4) == true and ui:cancel(q) == true and ui:answer(p.invite, true) == true
        and ui:cancel(p.post) == true and ui:answer('query', 1, 7) == true and ui:cancel('link5') == true
    local args = {}
    for _, c in ipairs(native_state.calls) do
        local a = {}
        for i = 1, c.n do a[i] = tostring(c.args[i]) end
        args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
    end
    check(done and table.concat(args, ' ') == 'answer(query,4,7) cancel(query,7) answer(prtyjoin,true,9)'
            .. ' cancel(delivery,3) answer(query,1,7) cancel(link5,nil)',
        'answer(prompt, value) and cancel(prompt) send the prompt\'s name and id; the old forms still work: '
            .. table.concat(args, ' '))
    native_state.pending = nil
end

-- a yes/no window's question: its text as UTF-8 with its bytes beside it as
-- raw and its segments' colors named; answer and cancel take its prompt
do
    native_state.calls = {}
    native_state.prompts = copy(prompts)
    native_state.prompts.comyn = {name = 'comyn', id = 11, text = 'Log out ' .. SJIS_A .. '?',
        segments = {run('Log out ', 1), run(SJIS_A, 2), run('?', 1)}, undecoded = 0, default = false,
        cancellable = true, hold = true}
    local o = ui:options('comyn')
    local seg = o.segments
    check(o.name == 'comyn' and o.id == 11 and o.text == 'Log out ' .. UTF8_A .. '?'
            and o.raw == 'Log out ' .. SJIS_A .. '?' and seg[2].text == UTF8_A and seg[2].raw == SJIS_A
            and seg[1].color == 'default' and seg[2].color == 'green' and seg[1].escape == nil
            and o.default == false and o.cancellable == true and o.hold == true and o.undecoded == 0,
        'options(comyn): the question as UTF-8, raw beside it and each segment, colors named, the rest as the engine'
            .. ' gave it')
    local done = ui:answer(o, true) == true and ui:answer(o, false) == true and ui:cancel(o) == true
        and ui:answer('resynus', true) == true
    local args = {}
    for _, c in ipairs(native_state.calls) do
        local a = {}
        for i = 1, c.n do a[i] = tostring(c.args[i]) end
        args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
    end
    check(done and table.concat(args, ' ') == 'answer(comyn,true,11) answer(comyn,false,11) cancel(comyn,11)'
            .. ' answer(resynus,true,nil)',
        'answer(prompt, true or false) and cancel(prompt) on a yes/no window send its name and id: '
            .. table.concat(args, ' '))
    native_state.prompts = prompts
end

-- mogdoor's question and a dialogue box's page: their texts as UTF-8 with
-- their bytes beside them as raw, mogdoor's rows and areas each; answer
-- takes mogdoor's exit and a dialogue box's prompt with no value
do
    native_state.calls = {}
    native_state.prompts = copy(prompts)
    native_state.prompts.mogdoor = {name = 'mogdoor', id = 41, level = 'top', blocked = false,
        title = {text = 'Leave ' .. SJIS_A .. '?', segments = {run('Leave ' .. SJIS_A .. '?', 1)}, undecoded = 0},
        options = {{text = 'Stay', kind = 'stay', enabled = true},
                   {text = 'Go out', kind = 'back', value = 0, enabled = true}},
        areas = {{text = 'Area ' .. SJIS_A, kind = 'area', value = 1, zone = 230, enabled = true}}}
    native_state.prompts.rem4line = {name = 'rem4line', id = 42, text = 'Moogle : ' .. SJIS_A .. '\nkupo!',
        segments = {run('Moogle : ' .. SJIS_A .. '\nkupo!', 1)}, undecoded = 0, waits = 'key', auto = false,
        more = true, queued = 0, cancellable = true}
    local door, talk = ui:options('mogdoor'), ui:options('rem4line')
    check(door.title.text == 'Leave ' .. UTF8_A .. '?' and door.title.raw == 'Leave ' .. SJIS_A .. '?'
            and door.areas[1].text == 'Area ' .. UTF8_A and door.areas[1].raw == 'Area ' .. SJIS_A
            and door.areas[1].value == 1 and door.options[2].text == 'Go out' and door.options[2].raw == 'Go out'
            and talk.text == 'Moogle : ' .. UTF8_A .. '\nkupo!' and talk.raw == 'Moogle : ' .. SJIS_A .. '\nkupo!'
            and talk.segments[1].text == 'Moogle : ' .. UTF8_A .. '\nkupo!' and talk.waits == 'key',
        'options(mogdoor): its title, rows and areas as UTF-8, raw beside each; options(rem4line): the page as'
            .. ' UTF-8, raw beside it')
    local done = ui:answer(door, 1) == true and ui:cancel(door) == true and ui:answer(talk) == true
        and ui:answer('rem4li2') == true and ui:cancel(talk) == true
    local args = {}
    for _, c in ipairs(native_state.calls) do
        local a = {}
        for i = 1, c.n do a[i] = tostring(c.args[i]) end
        args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
    end
    check(done and table.concat(args, ' ') == 'answer(mogdoor,1,41) cancel(mogdoor,41) answer(rem4line,nil,42)'
            .. ' answer(rem4li2,nil,nil) cancel(rem4line,42)',
        'answer(mogdoor, exit) and cancel send its name and id; answer(dialogue box) sends no value: '
            .. table.concat(args, ' '))
    native_state.prompts = prompts
end

-- the text windows: comment's lines, stringdl's prompt and msgline's
-- recipient as UTF-8 with their bytes beside them; answer passes a table
-- through as it is, with the prompt's name and id
do
    native_state.calls = {}
    native_state.prompts = copy(prompts)
    native_state.prompts.comment = {name = 'comment', id = 51, mode = 'bazaar',
        lines = {{text = 'Cheap ' .. SJIS_A}, {text = ''}, {text = 'kupo'}}}
    native_state.prompts.stringdl = {name = 'stringdl', id = 52, mode = 'name', min_length = 3, max_length = 15,
        prompt = 'To ' .. SJIS_A}
    native_state.prompts.msgline = {name = 'msgline', id = 53, mode = 0, max_length = 300,
        recipient = 'Friend' .. SJIS_A, blocked = false}
    local c, d, m = ui:options('comment'), ui:options('stringdl'), ui:options('msgline')
    check(c.lines[1].text == 'Cheap ' .. UTF8_A and c.lines[1].raw == 'Cheap ' .. SJIS_A and c.lines[2].raw == ''
            and d.prompt == 'To ' .. UTF8_A and d.prompt_raw == 'To ' .. SJIS_A
            and m.recipient == 'Friend' .. UTF8_A and m.recipient_raw == 'Friend' .. SJIS_A,
        'options(comment): each line as UTF-8, raw beside it; stringdl\'s prompt and msgline\'s recipient as'
            .. ' UTF-8, their bytes in prompt_raw and recipient_raw')
    local lines = {'Hello', 'World'}
    local done = ui:answer(c, lines) == true and ui:answer('link11', {name = 'Kupo', color = {1, 2, 3}}) == true
    local first, second = native_state.calls[1], native_state.calls[2]
    check(done and first.verb == 'answer' and first.args[1] == 'comment' and first.args[2] == lines
            and first.args[3] == 51 and second.args[1] == 'link11' and second.args[2].name == 'Kupo'
            and second.args[2].color[3] == 3,
        'answer(comment, lines) passes the table and the prompt\'s id through; answer(link11, {name, color}) its'
            .. ' table')
    native_state.prompts = prompts
end

-- the trade windows: offer and withdraw pass the item, 'gil' and the count
-- through; misuse raises at the addon's line; a released handle answers nil;
-- options' partner and npc as UTF-8 with their bytes beside them
do
    native_state.calls = {}
    native_state.prompts = copy(prompts)
    native_state.prompts.trade = {name = 'trade', id = 61, partner = 'Zald' .. SJIS_A,
        mine = {gil = 100, items = {{slot = 1, item = 3, id = 4096, count = 2},
                                   {slot = 2, item = 5, id = 4097, count = 1, pending = true}}},
        theirs = {gil = 0, items = {{slot = 1, id = 640, count = 12}}}, ok_mine = false, ok_theirs = true,
        next_change = 0.4, gray = {4, 9}}
    native_state.prompts.handover = {name = 'handover', id = 62, npc = 'Moogle' .. SJIS_A, gil = 0,
        items = {{slot = 1, item = 7, id = 1234, count = 1}}, next_ok = 0.7, gray = {}}
    local t, h = ui:options('trade'), ui:options('handover')
    check(t.partner == 'Zald' .. UTF8_A and t.partner_raw == 'Zald' .. SJIS_A and t.mine.items[2].pending == true
            and t.theirs.items[1].count == 12 and t.next_change == 0.4 and h.npc == 'Moogle' .. UTF8_A
            and h.npc_raw == 'Moogle' .. SJIS_A and h.items[1].item == 7 and #t.gray == 2 and t.gray[1] == 4
            and t.gray[2] == 9 and h.next_ok == 0.7 and #h.gray == 0,
        'options(trade) and options(handover): the partner and the NPC as UTF-8, their bytes in partner_raw and'
            .. ' npc_raw, the rest as the engine gave it, gray and next_ok among it')
    local done = ui:offer('trade', 3, 2) == true and ui:offer('Handover', 'gil', 500) == true
        and ui:withdraw('trade', 3) == true and ui:answer(t) == true and ui:cancel(h) == true
    local args = {}
    for _, c in ipairs(native_state.calls) do
        local a = {}
        for i = 1, c.n do a[i] = tostring(c.args[i]) end
        args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
    end
    check(done and table.concat(args, ' ') == 'offer(trade,3,2) offer(Handover,gil,500) withdraw(trade,3)'
            .. ' answer(trade,nil,61) cancel(handover,62)',
        'offer, withdraw, answer (the OK) and cancel reach the engine as given: ' .. table.concat(args, ' '))
    local raised = {}
    for _, attempt in ipairs({
            {function() here(); ui:offer('trade', 'silver', 1) end, "item must be an inventory index or 'gil'"},
            {function() here(); ui:offer('trade', 3) end, 'count must be a number'},
            {function() here(); ui:offer(3, 3, 1) end, 'name must be a string'},
            {function() here(); ui:withdraw('trade', 'gil') end, 'item must be an inventory index'},
            {function() here(); ui.offer('trade', 3, 1) end, 'use ui:offer'}}) do
        local at_line, text = raised_here(attempt[1])
        if at_line and text:find(attempt[2], 1, true) then
            raised[#raised + 1] = attempt[2]
        end
    end
    check(#raised == 5, 'offer and withdraw raise misuse at the addon\'s line: ' .. table.concat(raised, '; '))
    native_state.results.offer = {nil, 'all 8 item slots are full; withdraw one first'}
    local refused, why = ui:offer('trade', 9, 1)
    native_state.results.offer = {nil, 'that item cannot be traded'}
    local grayed, gray_why = ui:offer('trade', 4, 1)
    native_state.results.offer = nil
    check(refused == nil and why == 'all 8 item slots are full; withdraw one first' and grayed == nil
            and gray_why == 'that item cannot be traded',
        'a refused offer, a grayed item\'s too: nil and the engine\'s reason')
    native_state.prompts = prompts
end

-- the KO window: options() as the engine gave it; answer takes its prompt
-- and no value, and nil reaches the engine as no value
do
    native_state.calls = {}
    native_state.prompts = copy(prompts)
    native_state.prompts.dead = {name = 'dead', id = 5, seconds = 125, askable = true}
    local o = ui:options('dead')
    check(o.name == 'dead' and o.id == 5 and o.seconds == 125 and o.askable == true and o.text == nil
            and o.raw == nil,
        'options(dead): the countdown and askable as the engine gave them, nothing converted')
    local done = ui:answer(o) == true and ui:answer('dead') == true and ui:answer(o, true) == true
        and ui:cancel(o) == true
    local args = {}
    for _, c in ipairs(native_state.calls) do
        local a = {}
        for i = 1, c.n do a[i] = tostring(c.args[i]) end
        args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
    end
    check(done and table.concat(args, ' ') == 'answer(dead,nil,5) answer(dead,nil,nil) answer(dead,true,5)'
            .. ' cancel(dead,5)',
        'answer(prompt) and answer(name) with no value send nil for it, with the prompt\'s id or none: '
            .. table.concat(args, ' '))
    native_state.prompts = prompts
end

-- the number boxes: options() and pending() as the engine gave them, the
-- detail kept back with debug off; answer takes a number with the prompt
do
    native_state.calls = {}
    native_state.prompts = copy(prompts)
    native_state.prompts.moneyctr = {name = 'moneyctr', id = 21, value = 100, min = 0, max = 7000, blocked = false,
        detail = {asker = 'handover', callback = '0x1', context = '0x0'}}
    native_state.pending = {itemctrl = {name = 'itemctrl', id = 22, value = 1, min = 1, max = 12, blocked = true,
        item = 4660, detail = {asker = 'trade'}}}
    local o, p = ui:options('moneyctr'), ui:pending()
    check(o.name == 'moneyctr' and o.id == 21 and o.value == 100 and o.min == 0 and o.max == 7000
            and o.blocked == false and o.detail == nil and o.text == nil and o.raw == nil,
        'options(moneyctr): the number, its range and blocked as the engine gave them, no detail with debug off')
    local i = p.itemctrl
    check(i and i.name == 'itemctrl' and i.id == 22 and i.blocked == true and i.item == 4660 and i.detail == nil,
        'pending() lists a blocked itemctrl under its name, its item\'s id beside it')
    local done = ui:answer(o, 2500) == true and ui:cancel(o) == true and ui:answer(i, 5) == true
        and ui:cancel(i) == true and ui:answer('itemctrl', 3) == true
    local args = {}
    for _, c in ipairs(native_state.calls) do
        local a = {}
        for k = 1, c.n do a[k] = tostring(c.args[k]) end
        args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
    end
    check(done and table.concat(args, ' ') == 'answer(moneyctr,2500,21) cancel(moneyctr,21) answer(itemctrl,5,22)'
            .. ' cancel(itemctrl,22) answer(itemctrl,3,nil)',
        'answer(prompt, number) and cancel(prompt) on a number box send its name and id, from options() or'
            .. ' pending(): ' .. table.concat(args, ' '))
    native_state.prompts = prompts
    native_state.pending = nil
end

-- the Asked windows: their text as UTF-8 with the bytes beside it, the GM's
-- name, maplist's rows and labels, in options() and in pending(); answer and
-- cancel take each prompt, ok's with no value
do
    native_state.calls = {}
    native_state.prompts = copy(prompts)
    native_state.prompts.sortyn = {name = 'sortyn', id = 31, text = 'Drop ' .. SJIS_A .. '?',
        segments = {run('Drop ', 1), run(SJIS_A, 2), run('?', 1)}, undecoded = 0,
        title = {text = SJIS_A, segments = {run(SJIS_A, 1)}, undecoded = 0}, default = false, cancellable = true,
        blocked = false, detail = {callback = '0x1'}}
    native_state.prompts.gmtell = {name = 'gmtell', id = 36, gm = 'GM' .. SJIS_A, cancellable = true,
        title = {text = 'Are you there?', segments = {run('Are you there?', 1)}, undecoded = 0},
        options = {{text = SJIS_A, segments = {run(SJIS_A, 1)}, undecoded = 0, value = 1}}}
    native_state.prompts.maplist = {name = 'maplist', id = 35, top = true, blocked = false,
        rows = {{text = 'Area ' .. SJIS_A, value = 1, kind = 'map', zone = 100, map = 0, label = SJIS_A}}}
    native_state.prompts.mp_dead = {name = 'mp_dead', id = 37, seconds = 30,
        options = {{text = SJIS_A, segments = {run(SJIS_A, 1)}, undecoded = 0, value = 1}}}
    native_state.prompts.ok = {name = 'ok', id = 38, text = 'Lost ' .. SJIS_A, segments = {run('Lost ' .. SJIS_A, 1)},
        undecoded = 0, blocked = false}
    native_state.pending = {evitem01 = {name = 'evitem01', id = 32, text = 'Use ' .. SJIS_A .. '?',
        segments = {run('Use ' .. SJIS_A .. '?', 1)}, undecoded = 0, cancellable = true, blocked = true},
        bankmenu = {name = 'bankmenu', id = 34, blocked = true,
            containers = {{id = 1, available = true}, {id = 9, available = false}}}}
    local sortyn, gm, map = ui:options('sortyn'), ui:options('gmtell'), ui:options('maplist')
    local mp, ok, p = ui:options('mp_dead'), ui:options('ok'), ui:pending()
    check(sortyn.text == 'Drop ' .. UTF8_A .. '?' and sortyn.raw == 'Drop ' .. SJIS_A .. '?'
            and sortyn.segments[2].color == 'green' and sortyn.title.text == UTF8_A and sortyn.title.raw == SJIS_A
            and sortyn.detail == nil and sortyn.default == false,
        'options(sortyn): the question and its title as UTF-8, raw beside each, colors named, no detail with debug'
            .. ' off')
    check(gm.gm == 'GM' .. UTF8_A and gm.raw == 'GM' .. SJIS_A and gm.title.text == 'Are you there?'
            and gm.options[1].text == UTF8_A and gm.options[1].raw == SJIS_A,
        'options(gmtell): the GM\'s name as UTF-8, its bytes as raw; the title and options converted')
    local row = map.rows[1]
    check(row.text == 'Area ' .. UTF8_A and row.raw == 'Area ' .. SJIS_A and row.label == UTF8_A
            and row.label_raw == SJIS_A and row.zone == 100 and row.kind == 'map',
        'options(maplist): each row\'s text and label as UTF-8, their bytes beside them')
    check(mp.options[1].text == UTF8_A and mp.options[1].raw == SJIS_A and mp.seconds == 30
            and ok.text == 'Lost ' .. UTF8_A and ok.raw == 'Lost ' .. SJIS_A,
        'options(mp_dead) and options(ok): their texts as UTF-8')
    check(p.evitem01 and p.evitem01.text == 'Use ' .. UTF8_A .. '?' and p.evitem01.raw == 'Use ' .. SJIS_A .. '?'
            and p.bankmenu and p.bankmenu.containers[2].available == false,
        'pending(): each blocked question converted as options() converts it')
    local done = ui:answer(sortyn, true) == true and ui:cancel(p.evitem01) == true
        and ui:answer(p.bankmenu, 1) == true and ui:answer(map, 1) == true and ui:cancel(map) == true
        and ui:answer(gm, 1) == true and ui:answer(mp, 2) == true and ui:answer(ok) == true
    local args = {}
    for _, c in ipairs(native_state.calls) do
        local a = {}
        for k = 1, c.n do a[k] = tostring(c.args[k]) end
        args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
    end
    check(done and table.concat(args, ' ') == 'answer(sortyn,true,31) cancel(evitem01,32) answer(bankmenu,1,34)'
            .. ' answer(maplist,1,35) cancel(maplist,35) answer(gmtell,1,36) answer(mp_dead,2,37) answer(ok,nil,38)',
        'answer and cancel on the Asked windows send each prompt\'s name and id; ok\'s answer no value: '
            .. table.concat(args, ' '))
    native_state.prompts = prompts
    native_state.pending = nil
end

-- layout and apply: what this handle asked for, kept here
native_state.calls = {}
local layout = {groups = {party_list = {x = 1792, y = 800}, chat_log = {x = 16, y = 830}},
                positions = {targetwi = {x = 1500, y = 500}, equip = {x = 300, y = 200}},
                sizes = {partywin = {rows = 3}, equip = {w = 200, h = 100}}}
check(ui:apply(layout) == true, 'apply: true when every entry queues')
local order = {}
for _, c in ipairs(native_state.calls) do
    order[#order + 1] = c.verb .. ' ' .. table.concat({tostring(c.args[1]), tostring(c.args[2]), tostring(c.args[3])}, ',')
end
check(table.concat(order, ' | ') == 'move_group chat_log,16,830 | move_group party_list,1792,800 | move equip,300,200'
        .. ' | move targetwi,1500,500 | resize equip,200,100 | resize partywin,3,nil',
    'apply: the groups in the order they carry each other, then the moves, then the sizes by rows or w,h: '
        .. table.concat(order, ' | '))
native_state.results.move = {nil, 'no such window: nosuch'}
local refusals
r, why, refusals = ui:apply({positions = {nosuch = {x = 1, y = 2}}, groups = {bogus = {x = 1, y = 1}}})
native_state.results.move = nil
check(r == nil and why == 'move_group bogus: no such group (chat_log, party_list or target_window); move nosuch: no such'
        .. ' window: nosuch', 'apply: every refused entry, by what it was: ' .. tostring(why))
check(type(refusals) == 'table' and #refusals == 2 and refusals[1].verb == 'move_group' and refusals[1].name == 'bogus'
        and refusals[1].reason:find('no such group', 1, true) and refusals[2].verb == 'move'
        and refusals[2].name == 'nosuch' and refusals[2].reason == 'no such window: nosuch',
    'apply: and the list of them, each {verb, name, reason}')
local kept = ui:layout()
check(kept.groups.chat_log.x == 16 and kept.groups.party_list.y == 800 and kept.positions.equip.x == 300
        and kept.positions.targetwi.x == 1500 and kept.sizes.partywin.rows == 3 and kept.sizes.equip.w == 200
        and kept.positions.nosuch and kept.positions.nosuch.x == 1 and kept.groups.bogus.y == 1
        and kept.ui and kept.ui.w == 1920 and kept.ui.h == 1080 and (native_state.native_layouts or 0) == 0,
    'layout(): each entry apply() applied, as asked, the ones it could not beside them, and the UI size at the'
        .. ' last change; the engine\'s own layout is never read')
native_state.status.ui = {w = 2560, h = 1440}
local moved_now = ui:move('equip', 5, 6) == true and ui:layout().positions.equip.x == 5
local at_size = ui:layout().ui.w == 2560 and ui:layout().ui.h == 1440
native_state.status.ui = {w = 1920, h = 1080}
local other = hideui.new('other')
other:move('equip', 9, 9)
other:release()
check(moved_now and at_size and ui:layout().positions.equip.x == 5,
    'a move is in layout() at once, with the UI size it was made at; another handle\'s move of the window does not'
        .. ' change it')
check(ui:move('logwindo', 1, 1) and ui:move('partywin', 2, 2) and ui:move_group('chat_log', 20, 840) == true,
    'the log and the party list moved, then the chat log group')
kept = ui:layout()
check(kept.groups.chat_log.x == 20 and kept.groups.chat_log.y == 840 and kept.groups.party_list == nil
        and kept.positions.logwindo == nil and kept.positions.partywin == nil and kept.groups.bogus,
    'move_group chat_log takes over the entries it places again: the log\'s and the party list\'s own moves and'
        .. ' the party_list group')
check(ui:resize('equip', 300, 200) == true and ui:reset('equip', 'position') == true
        and ui:layout().positions.equip == nil and ui:layout().sizes.equip.w == 300
        and last_call('reset').args[2] == 'position',
    'reset(name, \'position\') reaches the engine with the aspect and forgets the position alone')
check(ui:move('equip', 7, 8) == true and ui:reset('equip', 'size') == true and ui:layout().positions.equip.x == 7
        and ui:layout().sizes.equip == nil,
    'reset(name, \'size\') forgets the size alone')
native_state.results.reset = {nil, 'equip was placed by other'}
r, why = ui:reset('equip')
native_state.results.reset = nil
check(r == nil and why == 'equip was placed by other' and ui:layout().positions.equip.x == 7,
    'a reset the engine refuses says who placed the window, and forgets nothing')
check(ui:reset('equip') == true and ui:layout().positions.equip == nil and last_call('reset').args[2] == nil,
    'reset(name) with no aspect')
check(ui:move_group('target_window', 1500, 600) == true and ui:reset_group('target_window') == true
        and ui:layout().groups.target_window == nil and last_call('reset_group').args[1] == 'target_window',
    'reset_group(group) reaches the engine and forgets the group\'s move')
native_state.results.resize = {nil, 'no template for 3 rows'}
ui:apply({sizes = {equip = {rows = 3}}, groups = {party_list = {x = 1700, y = 800}}})
native_state.results.resize = nil
native_state.results.move_group = {nil, 'move_group needs hideui 0.7.0 or newer'}
ui:apply({groups = {party_list = {x = 1700, y = 800}}})
native_state.results.move_group = nil
kept = ui:layout()
local before_reset = kept.sizes and kept.sizes.equip and kept.sizes.equip.rows == 3 and kept.groups.party_list
    and kept.groups.party_list.x == 1700
check(ui:reset('equip') == true and ui:reset('partywin') == true, 'reset of the windows whose entries were kept')
kept = ui:layout()
check(before_reset and kept.sizes == nil and kept.groups.party_list == nil and kept.groups.bogus,
    'a reset forgets the kept entries of its window, and of a group whose anchor it is; the others stay')
native_state.results.move = {nil, 'no such window: nosuch'}
ui:apply({groups = {bogus = {x = 1, y = 1}, chat_log = {x = 16, y = 830}}, positions = {nosuch = {x = 1, y = 2}}})
native_state.results.move = nil
check(ui:layout().positions.nosuch and ui:reset_all() == true and ui:layout().positions == nil
        and ui:layout().groups == nil and ui:layout().sizes == nil and ui:layout().ui.w == 1920,
    'reset_all forgets every entry, kept ones too; the UI size of that change stays')
native_state.calls = {}
native_state.status.ui = {w = 2560, h = 1440}
r, why = ui:apply({ui = {w = 1920, h = 1080}, positions = {equip = {x = 1, y = 2}}})
native_state.status.ui = {w = 1920, h = 1080}
check(r == nil and why == 'this layout was saved at 1920x1080; the game is at 2560x1440' and #native_state.calls == 0,
    'apply() refuses a layout saved at another UI size, and applies nothing: ' .. tostring(why))
check(ui:apply({ui = {w = 1920, h = 1080}, positions = {equip = {x = 1, y = 2}}}) == true
        and ui:layout().positions.equip.x == 1, 'and applies it at its own UI size')
check(ui:apply({}) == true, 'apply of an empty layout does nothing')
ok, err = pcall(ui.apply, ui, 'layout')
check(not ok and tostring(err):find('needs the table', 1, true), 'apply of a non-table raises')
ok, err = pcall(ui.apply, ui, {sizes = {equip = {w = 1}}})
check(not ok and tostring(err):find('sizes.equip needs', 1, true), 'apply of a malformed size raises: ' .. tostring(err))
ok, err = pcall(ui.apply, ui, {positions = 3})
check(not ok and tostring(err):find('layout.positions must be a table', 1, true), 'apply of a malformed section raises')
ok, err = pcall(ui.apply, ui, {ui = {w = '1920', h = 1080}})
check(not ok and tostring(err):find('layout.ui needs numbers w and h', 1, true), 'apply of a malformed ui raises')

local second = hideui.new('second')
local second_native = native_state.handles[#native_state.handles]
second:move_group('chat_log', 16, 830)
second:resize('partywin', 3)
check(second:release() == true and second_native.released, 'release reaches the engine')
local after, reason = second:hide('logwindo')
check(after == nil and reason:find('released'), 'a released handle answers nil and the reason')
do
    local calls_before = #native_state.calls
    local b, bwhy = second:block_macros()
    local u, uwhy = second:unblock_macros()
    local m, mwhy = second:macros()
    check(b == nil and bwhy:find('released') and u == nil and uwhy:find('released') and m == nil and mwhy:find('released')
            and #native_state.calls == calls_before,
        'block_macros, unblock_macros and macros on a released handle answer nil and the reason, reaching nothing')
end
local snapshot = second:layout()
snapshot.groups.chat_log.x = 0
local again = second:layout()
check(again.groups.chat_log.x == 16 and again.groups.chat_log.y == 830 and again.sizes.partywin.rows == 3
        and again.positions == nil and again.ui.w == 1920 and second:info('equip') == nil,
    'layout() on a released handle: what it asked for, unchanged by the release, a fresh copy each time; other'
        .. ' reads refuse')
second_native.queue = {{event = 'opened', name = 'x'}}
fire('prerender')
check(#second_native.queue == 1, 'a released handle is no longer polled')

native_state.new_error = "hideui can't work with this version of FFXI; update the addon"
native_state.status = status_table(false, 'signature open_by_name: not found')
local broken, broken_why = hideui.new('broken')
check(broken == nil and broken_why == native_state.new_error and #chat == 0,
    'new on an engine that cannot install: nil and the reason, nothing printed')
hideui.debug(true)
hideui.new('broken')
check(chat_has("broken: hideui can't work with this version of FFXI; update the addon")
        and chat_has('broken: signature open_by_name: not found'),
    'with debug on, new prints the reason and the detail')
hideui.debug(false)
native_state.new_error = nil
native_state.status = status_table(true)

chat = {}
native_state.shutdown = {false, 'the command queue is full; the engine stays installed', 'drain'}
local at_unload
windower.register_event('unload', function() at_unload = ui:layout() end)
ui:move('equip', 300, 200)
fire('unload')
check(first_native.released and native_state.shutdown_called == 1, 'unload releases every handle, then asks the engine to shut down')
check(at_unload and at_unload.positions.equip.x == 300 and at_unload.positions.equip.y == 200,
    'an addon\'s own unload handler, run after hideui.lua\'s, still reads its layout: the one taken at release')
check(#chat == 0, 'a shutdown the engine could not finish is not printed with debug off')

reset_windower()
hideui = dofile(addon_dir .. '/libs/hideui.lua')
hideui.new('third')
hideui.debug(true)
native_state.shutdown = {false, '1 hideui handle(s) still open', 'handles'}
fire('unload')
local quiet = #chat == 0
native_state.shutdown = {false, 'the command queue is full; the engine stays installed', 'drain'}
fire('unload')
check(quiet and chat_has('queue is full'),
    'with debug on, a shutdown the engine could not finish is printed; another addon still holding handles is not')

-- the reference addon -------------------------------------------------------------

local saved_settings, saves = nil, 0
package.loaded.config = {
    load = function(defaults)
        saved_settings = saved_settings or defaults
        return saved_settings
    end,
    save = function(t)
        saved_settings = t
        saves = saves + 1
    end,
}

reset_windower()
_addon = {}
package.path = addon_dir .. '/?.lua;' .. package.path
package.loaded['libs.hideui'] = nil
dofile(addon_dir .. '/hideuidemo.lua')
check(_addon.name == 'hideuidemo' and _addon.commands[1] == 'hideuidemo', 'hideuidemo declares its command')
local command = handlers['addon command'][1]
native_state.pending = {invite = {name = 'prtyjoin', inviter = 'Zaldon', alliance = false, id = 9},
                        post = {name = 'delivery', box = 'delivery', id = 3, detail = {state = 5}}}
local commands = {
    {'hide', 'logwindo'}, {'unhide', 'logwindo'}, {'block', 'menuwind'}, {'unblock', 'menuwind'},
    {'block', 'ability', 'pet_commands'}, {'unblock', 'ability', 'Pet_Commands'}, {'block'},
    {'events', 'on', 'ability', 'pet_commands'},
    {'move', 'equip', '300', '200'}, {'group', 'chat_log', '16', '830'}, {'reset', 'equip'},
    {'reset', 'equip', 'position'}, {'reset', 'equip', 'size'}, {'reset', 'equip', 'all'}, {'reset'},
    {'resetgroup', 'chat_log'}, {'resetgroup'}, {'rects'},
    {'resetall'}, {'open', 'equip'}, {'open', 'Ability', 'Pet_Commands'}, {'openset', 'equipment'},
    {'openset', 'magic', 'white_magic'}, {'openset'}, {'open', 'iteminfo', 'Bag=inventory', 'slot=5'},
    {'openset', 'region', 'colonization'}, {'close', 'equip'}, {'info', 'equip'}, {'info', 'query'},
    {'info', 'ability'},
    {'list'},
    {'list', 'log'},
    {'opened'}, {'focused'}, {'remembered'}, {'groups'}, {'info', 'arealist'}, {'resize', 'partywin', '3'},
    {'resize', 'equip', '300', '200'}, {'resize', 'partywin', 'ptw3'},
    {'options', 'query'}, {'options', 'link5'}, {'options', 'arealist'}, {'options', 'passinpu'},
    {'options', 'prtyjoin'}, {'options', 'equip'}, {'options'},
    {'answer', 'query', '4'}, {'answer', 'passinpu', 'two', 'words'}, {'answer', 'prtyjoin', 'yes'},
    {'answer', 'link5', '3'}, {'answer', 'arealist', '231'}, {'answer', 'prtyjoin', 'maybe'}, {'answer'},
    {'cancel', 'query'}, {'cancel', 'delivery'}, {'cancel'}, {'pending'}, {'answer', 'prtyjoin', 'no'},
    {'cancel', 'delivery'}, {'blockmacros'}, {'macros'}, {'unblockmacros'},
    {'move', 'equip', '300', '200'}, {'group', 'chat_log', '16', '830'},
    {'layout'}, {'apply'}, {'events', 'on'}, {'debug', 'on'}, {'debug', 'off'}, {'status'}, {'help'},
    {'move', 'equip'},
    {'settings'}, {'settings', 'color'}, {'settings', 'nothing_matches'}, {'setting', 'color_say'},
    {'setting', 'nosuch'}, {'setting'}, {'set', 'color_say', '160', '64', '160'}, {'set', 'footstep_effects', 'off'},
    {'set', 'shadows', 'high'}, {'set', 'music_volume', '40'}, {'set', 'key1_move_up', '30'}, {'set', 'color_say', '1'},
    {'set'},
}
local errors_seen = {}
for _, c in ipairs(commands) do
    local done, failure = pcall(command, unpack(c))
    if not done then errors_seen[#errors_seen + 1] = table.concat(c, ' ') .. ': ' .. tostring(failure) end
end
check(#errors_seen == 0, 'every hideuidemo command runs without a Lua error'
    .. (#errors_seen > 0 and (' -- ' .. errors_seen[1]) or ''))
check(chat_has('move equip to 300,200') and chat_has('usage: //hideuidemo move') and chat_has('move_group chat_log to 16,830'),
    'hideuidemo reports outcomes and usage')
do
    local sets = {}
    for _, c in ipairs(native_state.calls) do
        if c.verb == 'set_setting' then
            local v = c.args[2]
            if type(v) == 'table' then v = ('%d %d %d'):format(v.r, v.g, v.b) end
            sets[#sets + 1] = c.args[1] .. '=' .. tostring(v)
        end
    end
    check(table.concat(sets, ' ', #sets - 4) == 'color_say=160 64 160 footstep_effects=false shadows=high'
            .. ' music_volume=40 key1_move_up=30'
            and chat_has('color_say [conftxtc color] 128 128 128 (r g b 0..255, default 128 128 128): Say ' .. SJIS_A)
            and chat_has('music_volume [conf2win number] 100 (0..100, default 100)')
            and chat_has('shadows [conf3win choice] normal (high|normal|off, default normal)')
            and chat_has('footstep_effects [conf6win bool] on (on|off, default on)')
            and chat_has('color_say = 128 128 128') and chat_has('no setting matches nothing_matches')
            and chat_has('setting nosuch failed: no such setting: nosuch') and chat_has('usage: //hideuidemo set <name>')
            and chat_has('usage: //hideuidemo setting <name>') and chat_has('set shadows high'),
        'hideuidemo settings prints names, kinds, values, ranges and labels (the game\'s bytes, as every chat line),'
            .. ' with a filter; setting prints one; set gives a color as r g b, a bool as on|off, a choice by name, a'
            .. ' number and a key as numbers: ' .. table.concat(sets, ' '))
end
do
    local before = #native_state.calls
    native_state.settings_fail = "the game's settings are not loaded: the player is not in the game"
    local done = pcall(command, 'set', 'footstep_effects', 'on')
    native_state.settings_fail = nil
    local sent = false
    for k = before + 1, #native_state.calls do
        sent = sent or native_state.calls[k].verb == 'set_setting'
    end
    check(done and not sent
            and chat_has("set failed: the game's settings are not loaded: the player is not in the game"),
        'hideuidemo set while settings() fails: its reason reported, nothing sent')
    pcall(command, 'set', 'no_such_name', '5')
    local last = native_state.calls[#native_state.calls]
    check(last.verb == 'set_setting' and last.args[1] == 'no_such_name' and last.args[2] == 5,
        'hideuidemo set of a name settings() does not list: sent as typed, for set_setting to answer')
end
check(chat_has('resizes by rows 1..6') and chat_has('until its owner re-sizes it') and chat_has('"Party"')
        and chat_has(' focused') and chat_has('hidden by hideuidemo, other'),
    'hideuidemo info shows the rows it resizes by, how long a size holds, focus, element text and who hides it')
check(chat_has('rect 1,2 3x4, cursor 1') and chat_has('cursor 2 of 3 (top 1)') and chat_has('cursor 3 of 3 (top 2)'),
    'hideuidemo info prints a window\'s cursor row, and query\'s and arealist\'s as the option or row under it of'
        .. ' all options(name) lists, with the first shown')
check(chat_has('  category 2 (pet_commands)') and not chat_has('  category 1'),
    'hideuidemo info prints the list ability shows, its category number and name, and nothing for other windows')
do
    local lists = {}
    for _, c in ipairs(native_state.calls) do
        if (c.verb == 'block' or c.verb == 'unblock') and c.args[1] == 'ability' then
            lists[#lists + 1] = c.verb .. ':' .. tostring(c.args[2]) .. ':' .. c.n
        end
    end
    local opens = {}
    for _, c in ipairs(native_state.calls) do
        if c.verb == 'open' or c.verb == 'open_set' then
            opens[#opens + 1] = c.verb .. ':' .. tostring(c.args[1]) .. ':' .. tostring(c.args[2]) .. ':' .. c.n
        end
    end
    check(table.concat(opens, ' ') == 'open:equip:nil:1 open:ability:pet_commands:2 open_set:equipment:nil:1'
            .. ' open_set:magic:white_magic:2 open:iteminfo:bag=inventory;slot=5:2'
            .. ' open_set:region:colonization:2' and chat_has('open_set equipment')
            and chat_has('usage: //hideuidemo openset <set> [argument]'),
        'hideuidemo open takes an argument after the name, key=value words as a table, openset a set and an'
            .. ' argument, all lower-cased: '
            .. table.concat(opens, ' '))
    check(table.concat(lists, ' ') == 'block:pet_commands:2 unblock:pet_commands:2' and chat_has('block ability pet_commands')
            and chat_has('unblock ability pet_commands') and chat_has('usage: //hideuidemo block <name> [category]')
            and chat_has('events on: ability pet_commands'),
        'hideuidemo block and unblock pass a category, lower-cased, and events on takes one after the name: '
            .. table.concat(lists, ' '))
end
check(chat_has('3: cursor at 5,6') and not chat_has('3: cursor at 5,6 size') and chat_has('4: other')
        and not chat_has('4: other at'),
    'hideuidemo info prints the element types by name, and only the box fields an element has')
check(chat_has('remembered at 5,6 owner hideuidemo (mine), size 200x100 owner other')
        and chat_has('partywin at 1792,830 (group chat_log) owner hideuidemo (mine), size 3 rows owner hideuidemo (mine)'),
    'hideuidemo shows remembered positions, groups and sizes in info and remembered')
check(chat_has('query "Will you lend a hand?": 3 options, cancellable, prompt 7')
        and chat_has('1: "Yes, gladly." -- answer 1, "gladly" in green') and chat_has('2: "Not today." -- answer 4')
        and chat_has('3: "?Cancel?" -- answer 5 (2 two-byte characters shown as ?)') and not chat_has('[green]')
        and not chat_has('[1F 79]'),
    'hideuidemo options query prints the plain text, notes a green run as , "word" in green, and says how many'
        .. ' characters it could not decode')
check(chat_has('slot 3: Shell One') and chat_has('arealist mode 0 level 0: 3 rows, prompt 4')
        and chat_has('1: "Current Area" [here] -- answer 4 (current_area)')
        and chat_has('2: "Region 01" [1] -- answer -2 (region)') and chat_has('3: "Zone 231" [Z231] -- answer 231 (zone)')
        and chat_has('at most 16 bytes')
        and chat_has('prtyjoin: alliance invite from Zaldon') and chat_has('options failed: equip has no options'),
    'hideuidemo options prints link5\'s slots, arealist\'s rows with their label or count, id and kind, passinpu\'s'
        .. ' limit, prtyjoin\'s invite, and why a window has none')
check(chat_has('chat_log: x (open at 16,930), 2 windows move with it')
        and chat_has('party_list: x (closed), 2 windows')
        and chat_has('a move to 1700,800 waits for it to open, owner hideuidemo (mine)'),
    'hideuidemo groups prints each anchor\'s origin while it is open, and a move waiting for a closed one')
check(chat_has('party invite from Zaldon') and chat_has('post-box session open: delivery')
        and not chat_has('post-box session open: delivery,'),
    'hideuidemo pending prints the invite and the post-box session, with no state word')
check(chat_has('focused: equip') and chat_has('2 open:'), 'hideuidemo focused and opened')

local answers, ids = {}, {}
for _, c in ipairs(native_state.calls) do
    if c.verb == 'answer' then
        answers[#answers + 1] = tostring(c.args[1]) .. '=' .. type(c.args[2]) .. ':' .. tostring(c.args[2])
    end
    if c.verb == 'answer' or c.verb == 'cancel' then
        ids[#ids + 1] = c.verb .. ' ' .. tostring(c.args[1]) .. '=' .. tostring(c.args[3] or c.args[2])
    end
end
check(table.concat(answers, ' ') == 'query=number:4 passinpu=string:two words prtyjoin=boolean:true link5=number:3'
        .. ' arealist=number:231 prtyjoin=boolean:false',
    'hideuidemo answer passes each window the type it takes: ' .. table.concat(answers, ' '))
check(table.concat(ids, ' ') == 'answer query=7 answer passinpu=1 answer prtyjoin=5 answer link5=2 answer arealist=4'
        .. ' cancel query=7 cancel delivery=nil answer prtyjoin=9 cancel delivery=3' and chat_has('answer query 4 (prompt 7)'),
    'hideuidemo answer and cancel reply to the prompt the last options or pending read of it gave, by its id, and'
        .. ' name a window none was read for: ' .. table.concat(ids, ' '))
local resets = {}
for _, c in ipairs(native_state.calls) do
    if c.verb == 'reset' or c.verb == 'reset_group' then
        resets[#resets + 1] = c.verb .. ' ' .. tostring(c.args[1]) .. ' ' .. tostring(c.args[2])
    end
end
check(table.concat(resets, ', ') == 'reset equip nil, reset equip position, reset equip size, reset_group chat_log nil'
        and chat_has('usage: //hideuidemo reset <name> [position|size]') and chat_has('usage: //hideuidemo resetgroup'),
    'hideuidemo reset takes an aspect, resetgroup a group, and a bad aspect or a missing name gets the usage: '
        .. table.concat(resets, ', '))
check(chat_has('2 open:') and chat_has('  equip 300,200 200x160') and chat_has('  logwindo 16,898 1774x166'),
    'hideuidemo rects prints every open window\'s frame')
local saved_layout = saved_settings and saved_settings.layout or {}
check(saves == 1 and saved_layout.groups.chat_log.x == 16 and saved_layout.sizes.partywin.rows == 3
        and saved_layout.ui.w == 1920 and chat_has('group chat_log 16,830') and chat_has('at UI 1920x1080')
        and chat_has('move equip 300,200') and chat_has('size partywin 3 rows'),
    'hideuidemo layout prints what the handle asked for, with the UI size, and saves it through the config library')
local applied = {}
for _, c in ipairs(native_state.calls) do
    if c.verb == 'move_group' or c.verb == 'move' or c.verb == 'resize' then
        applied[#applied + 1] = c.verb .. ' ' .. tostring(c.args[1])
    end
end
check(table.concat(applied, ' '):find('move_group chat_log move equip resize equip resize partywin', 1, true),
    'hideuidemo apply replays the saved layout through apply(): ' .. table.concat(applied, ' '))
chat = {}
native_state.results.resize = {nil, 'no template for 3 rows'}
command('apply')
native_state.results.resize = nil
check(chat_has('apply: 2 entries not applied, kept in the layout:') and chat_has('  resize partywin: no template for 3 rows'),
    'hideuidemo apply prints each entry it could not apply, from apply()\'s list')
chat = {}
native_state.status.ui = {w = 2560, h = 1440}
command('apply')
native_state.status.ui = {w = 1920, h = 1080}
check(chat_has('apply failed: this layout was saved at 1920x1080; the game is at 2560x1440'),
    'hideuidemo apply says a layout saved at another UI size was not applied')
local forms = {}
for _, c in ipairs(native_state.calls) do
    if c.verb == 'resize' then forms[#forms + 1] = table.concat({tostring(c.args[1]), tostring(c.args[2]),
        tostring(c.args[3])}, ',') end
end
local seen_forms = table.concat(forms, ' ')
check(seen_forms:find('partywin,3,nil', 1, true) and seen_forms:find('equip,300,200', 1, true)
        and not seen_forms:find('ptw3', 1, true),
    'hideuidemo passes resize as numbers (rows, or w and h): ' .. seen_forms)
do
    local macro_calls = {}
    for _, c in ipairs(native_state.calls) do
        if c.verb == 'block_macros' or c.verb == 'unblock_macros' then macro_calls[#macro_calls + 1] = c.verb end
    end
    chat = {}
    command('macros')
    local unblocked = chat_has('macro keys not blocked')
    native_state.macros = {blocked = true, blocked_by = {'hideuidemo', 'other'}, mine = true}
    command('macros')
    native_state.macros = nil
    check(table.concat(macro_calls, ' ') == 'block_macros unblock_macros' and unblocked
            and chat_has('macro keys blocked by hideuidemo, other (mine)'),
        'hideuidemo blockmacros and unblockmacros reach the engine, and macros prints whether the keys are blocked'
            .. ' and who blocks them: ' .. table.concat(macro_calls, ' '))
end

chat = {}
native_state.prompts = copy(prompts)
native_state.prompts.arealist = {name = 'arealist', id = 6, mode = 1, level = 0, pending = true, rows = {}}
command('options', 'arealist')
command('cancel', 'post1')
native_state.prompts = prompts
check(chat_has("arealist blocked, an NPC's prompt waiting (mode 1), prompt 6: answer arealist <zone id 0..511> or"
            .. ' cancel arealist') and chat_has('cancel post1') and chat_has('  the box closes when the server answers'),
    'hideuidemo options prints a blocked arealist prompt, no rows and pending, as the answer it waits on; cancel of'
        .. ' a post box says the box closes when the server answers')

chat = {}
native_state.calls = {}
native_state.prompts = copy(prompts)
native_state.prompts.comyn = {name = 'comyn', id = 11, text = 'Log out?', segments = {run('Log out?', 1)},
    undecoded = 0, default = false, cancellable = true, hold = false}
command('options', 'comyn')
command('answer', 'comyn', 'yes')
command('answer', 'comyn', 'maybe')
native_state.prompts = prompts
do
    local args = {}
    for _, c in ipairs(native_state.calls) do
        local a = {}
        for i = 1, c.n do a[i] = tostring(c.args[i]) end
        args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
    end
    check(chat_has('comyn "Log out?": no first, cancellable, prompt 11') and chat_has('answer comyn yes|no or cancel comyn')
            and table.concat(args, ' ') == 'answer(comyn,true,11)' and chat_has('usage: //hideuidemo answer'),
        'hideuidemo options prints a yes/no question, answer takes yes or no for it and the prompt the options'
            .. ' read, and anything else is a usage: ' .. table.concat(args, ' '))
end

chat = {}
native_state.calls = {}
native_state.prompts = copy(prompts)
native_state.prompts.dead = {name = 'dead', id = 5, seconds = 125, askable = true}
command('options', 'dead')
command('answer', 'dead')
command('answer', 'dead', 'yes')
command('cancel', 'dead')
native_state.prompts.dead = {name = 'dead', id = 6, seconds = 59, askable = false}
command('options', 'dead')
native_state.prompts = prompts
do
    local args = {}
    for _, c in ipairs(native_state.calls) do
        local a = {}
        for i = 1, c.n do a[i] = tostring(c.args[i]) end
        args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
    end
    check(chat_has('dead: 2:05 on the countdown, prompt 5') and chat_has('answer dead asks its question through comyn')
            and chat_has('dead: 0:59 on the countdown, prompt 6') and chat_has('its question cannot be asked now')
            and chat_has('answer dead (prompt 5)') and table.concat(args, ' ') == 'answer(dead,nil,5) cancel(dead,5)'
            and chat_has('usage: //hideuidemo answer'),
        'hideuidemo options prints the KO window\'s countdown and whether its question can be asked; answer dead takes'
            .. ' no value and the prompt the options read, and anything after it is a usage: ' .. table.concat(args, ' '))
end
do
    local lines = {}
    for _, line in ipairs({'options <name>', 'answer <name> <value>', 'cancel <name>'}) do
        chat = {}
        command('help')
        for _, l in ipairs(chat) do
            if l.text:find('//hideuidemo ' .. line, 1, true) and l.text:find('dead', 1, true) then
                lines[#lines + 1] = line
            end
        end
    end
    check(#lines == 3, 'hideuidemo help names dead under options, answer and cancel: ' .. table.concat(lines, ', '))
end

chat = {}
native_state.calls = {}
native_state.prompts = copy(prompts)
native_state.prompts.moneyctr = {name = 'moneyctr', id = 21, value = 100, min = 0, max = 7000, blocked = false}
native_state.pending = {itemctrl = {name = 'itemctrl', id = 22, value = 1, min = 1, max = 12, blocked = true,
    item = 4660}}
command('options', 'moneyctr')
command('answer', 'moneyctr', '2500')
command('answer', 'moneyctr', 'lots')
command('pending')
command('cancel', 'itemctrl')
command('block', 'itemctrl')
native_state.prompts = prompts
native_state.pending = nil
do
    local args = {}
    for _, c in ipairs(native_state.calls) do
        if c.verb ~= 'block' then
            local a = {}
            for i = 1, c.n do a[i] = tostring(c.args[i]) end
            args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
        end
    end
    check(chat_has('moneyctr: 100 (0..7000), prompt 21') and chat_has('answer moneyctr <0..7000> or cancel moneyctr')
            and chat_has('itemctrl: 1 (1..12) of item 4660, blocked: no window, prompt 22')
            and chat_has('usage: //hideuidemo answer') and chat_has('cancel itemctrl (prompt 22)')
            and chat_has('each question waits with no window until you answer or cancel it')
            and table.concat(args, ' ') == 'answer(moneyctr,2500,21) cancel(itemctrl,22)',
        'hideuidemo options and pending print a number box\'s question, answer takes a number for it and the prompt'
            .. ' read, cancel the prompt pending read, and block says what it does: ' .. table.concat(args, ' '))
end
do
    local lines = {}
    for _, line in ipairs({'options <name>', 'answer <name> <value>', 'cancel <name>', 'pending'}) do
        chat = {}
        command('help')
        for _, l in ipairs(chat) do
            if l.text:find('//hideuidemo ' .. line, 1, true)
                    and (l.text:find('moneyctr', 1, true) or l.text:find('number box', 1, true)) then
                lines[#lines + 1] = line
            end
        end
    end
    check(#lines == 4, 'hideuidemo help names the number boxes under options, answer, cancel and pending: '
        .. table.concat(lines, ', '))
end

chat = {}
native_state.calls = {}
native_state.prompts = copy(prompts)
native_state.prompts.sortyn = {name = 'sortyn', id = 31, text = 'Drop the Rusty Key?',
    segments = {run('Drop the ', 1), run('Rusty Key', 2), run('?', 1)}, undecoded = 0,
    title = {text = 'Drop', segments = {run('Drop', 1)}, undecoded = 0}, default = false, cancellable = true,
    blocked = false}
native_state.prompts.confyn = {name = 'confyn', id = 33, default = true}
native_state.prompts.maplist = {name = 'maplist', id = 35, top = true, blocked = false,
    rows = {{text = 'Current Area', value = 1, kind = 'map', zone = 100, map = 0, label = 'East'},
            {text = 'Ronfaure', value = 2, kind = 'list', count = 6},
            {text = 'Close', value = 3, kind = 'close'}}}
native_state.prompts.gmtell = {name = 'gmtell', id = 36, gm = 'GM_Test', cancellable = true,
    title = {text = 'Are you there?', segments = {run('Are you there?', 1)}, undecoded = 0},
    options = {{text = 'Yes', segments = {run('Yes', 1)}, undecoded = 0, value = 1},
               {text = 'No', segments = {run('No', 1)}, undecoded = 0, value = 2}}}
native_state.prompts.mp_dead = {name = 'mp_dead', id = 37, seconds = 30,
    options = {{text = 'Retry', segments = {run('Retry', 1)}, undecoded = 0, value = 1},
               {text = 'Return', segments = {run('Return', 1)}, undecoded = 0, value = 2}}}
native_state.prompts.ok = {name = 'ok', id = 38, text = 'Lost connection.', segments = {run('Lost connection.', 1)},
    undecoded = 0, blocked = false}
native_state.pending = {evitem01 = {name = 'evitem01', id = 32, text = 'Use the Rusty Key?',
    segments = {run('Use the Rusty Key?', 1)}, undecoded = 0, cancellable = true, blocked = true},
    bankmenu = {name = 'bankmenu', id = 34, blocked = true,
        containers = {{id = 1, available = true}, {id = 9, available = false}}}}
command('options', 'sortyn')
command('answer', 'sortyn', 'yes')
command('options', 'confyn')
command('answer', 'confyn', 'no')
command('options', 'maplist')
command('answer', 'maplist', '1')
command('options', 'gmtell')
command('answer', 'gmtell', '2')
command('cancel', 'gmtell')
command('options', 'mp_dead')
command('answer', 'mp_dead', '2')
command('options', 'ok')
command('answer', 'ok', 'now')
command('answer', 'ok')
command('pending')
command('cancel', 'evitem01')
command('answer', 'bankmenu', '1')
command('block', 'bankmenu')
native_state.prompts = prompts
native_state.pending = nil
do
    local args = {}
    for _, c in ipairs(native_state.calls) do
        if c.verb ~= 'block' then
            local a = {}
            for i = 1, c.n do a[i] = tostring(c.args[i]) end
            args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
        end
    end
    check(chat_has('sortyn "Drop the Rusty Key?", no first, prompt 31, "Rusty Key" in green')
            and chat_has('  title "Drop"') and chat_has('answer sortyn yes|no or cancel sortyn')
            and chat_has('confyn: restore the defaults? yes first, prompt 33') and chat_has('answer confyn yes|no')
            and chat_has('maplist: 3 rows, the first level, prompt 35')
            and chat_has('1: "Current Area" [East] -- answer 1 (zone 100 map 0)')
            and chat_has('2: "Ronfaure" [6] -- a list of its own')
            and chat_has('3: "Close" -- answer 3 (close)')
            and chat_has('gmtell from GM_Test: "Are you there?", 2 options, prompt 36')
            and chat_has('2: "No" -- answer 2') and chat_has('mp_dead: 0:30 on the countdown, prompt 37')
            and chat_has('2: "Return"') and chat_has('answer mp_dead 1|2')
            and chat_has('ok "Lost connection.", prompt 38')
            and chat_has('evitem01 "Use the Rusty Key?", blocked: no window, prompt 32')
            and chat_has('bankmenu: containers 1, 9 (not available), blocked: no window, prompt 34')
            and chat_has('no other Mog Safe picker opens until you answer or cancel the one waiting')
            and chat_has('usage: //hideuidemo answer')
            and table.concat(args, ' ') == 'answer(sortyn,true,31) answer(confyn,false,33) answer(maplist,1,35)'
                .. ' answer(gmtell,2,36) cancel(gmtell,36) answer(mp_dead,2,37) answer(ok,nil,38)'
                .. ' cancel(evitem01,32) answer(bankmenu,1,34)',
        'hideuidemo options and pending print each Asked window\'s question in its shape, answer takes yes|no, a'
            .. ' number or no value as each takes it with the prompt read, a value after ok is a usage, and block'
            .. ' says what it does: ' .. table.concat(args, ' '))
end
do
    local lines = {}
    for _, line in ipairs({'options <name>', 'answer <name> <value>', 'cancel <name>', 'pending'}) do
        chat = {}
        command('help')
        for _, l in ipairs(chat) do
            if l.text:find('//hideuidemo ' .. line, 1, true) and l.text:find('maplist', 1, true)
                    and l.text:find('bankmenu', 1, true) then
                lines[#lines + 1] = line
            end
        end
    end
    local all = {}
    chat = {}
    command('help')
    for _, w in ipairs({'sortyn', 'evitem01', 'confyn', 'bankmenu', 'maplist', 'gmtell', 'mp_dead', 'ok'}) do
        for _, l in ipairs(chat) do
            if l.text:find('//hideuidemo options', 1, true) and l.text:find(w, 1, true) then
                all[#all + 1] = w
                break
            end
        end
    end
    check(#lines == 4 and #all == 8, 'hideuidemo help names the Asked windows under options, answer, cancel and'
        .. ' pending, every one under options: ' .. table.concat(lines, ', ') .. '; ' .. table.concat(all, ' '))
end

-- hideuidemo's maplist below its first level: the level named, a list row
-- answered to open it, the cancel back up a level
do
    chat = {}
    native_state.calls = {}
    native_state.prompts = copy(prompts)
    native_state.prompts.maplist = {name = 'maplist', id = 39, top = false, level = 'region', blocked = false,
        rows = {{text = 'West Ronfaure', value = 1, kind = 'map', zone = 100, map = 1},
                {text = 'King Ranperre\'s Tomb', value = 2, kind = 'list'}}}
    command('options', 'maplist')
    command('answer', 'maplist', '2')
    command('cancel', 'maplist')
    local args = {}
    for _, c in ipairs(native_state.calls) do
        local a = {}
        for i = 1, c.n do a[i] = tostring(c.args[i]) end
        args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
    end
    check(chat_has('maplist: 2 rows, a region\'s list, prompt 39')
            and chat_has('1: "West Ronfaure" -- answer 1 (zone 100 map 1)')
            and chat_has('2: "King Ranperre\'s Tomb" -- a list of its own, answer 2 opens it')
            and chat_has('answer maplist <row value> or cancel maplist (back up a level)')
            and chat_has('each level is a prompt of its own')
            and table.concat(args, ' ') == 'answer(maplist,2,39) cancel(maplist,39)',
        'hideuidemo options maplist below its first level: the level named, a list row answered to open it, the'
            .. ' cancel back up a level: ' .. table.concat(args, ' '))
    native_state.prompts = prompts
end

chat = {}
native_state.calls = {}
native_state.prompts = copy(prompts)
native_state.prompts.mogdoor = {name = 'mogdoor', id = 41, level = 'top', blocked = false,
    title = {text = 'Leave the Mog House?'},
    options = {{text = 'Stay', kind = 'stay', enabled = true}, {text = 'Go out', kind = 'back', value = 0,
               enabled = true}, {text = 'Second floor', kind = 'floor', value = 125, enabled = false},
               {text = 'Other areas', kind = 'areas', enabled = true}},
    areas = {{text = 'Port Town', kind = 'area', value = 1, zone = 232, enabled = true},
             {text = 'Mog Garden', kind = 'garden', value = 127, zone = 280, enabled = true}}}
native_state.prompts.rem4line = {name = 'rem4line', id = 42, text = 'Moogle : Welcome home,\nkupo!',
    segments = {run('Moogle : Welcome home,\nkupo!', 1)}, undecoded = 0, waits = 'key', auto = false, more = true,
    queued = 0, cancellable = true}
native_state.prompts.rem4li2 = {name = 'rem4li2', id = 43, text = 'Rest well.', segments = {run('Rest well.', 1)},
    undecoded = 0, waits = 'timer', seconds = 2.5, auto = false, more = false, queued = 1, cancellable = false}
command('options', 'mogdoor')
command('answer', 'mogdoor', '127')
command('cancel', 'mogdoor')
command('options', 'rem4line')
command('answer', 'rem4line')
command('answer', 'rem4line', 'now')
command('options', 'rem4li2')
command('cancel', 'rem4line')
do
    local args = {}
    for _, c in ipairs(native_state.calls) do
        local a = {}
        for i = 1, c.n do a[i] = tostring(c.args[i]) end
        args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
    end
    check(chat_has('mogdoor "Leave the Mog House?": top level, prompt 41') and chat_has('1: "Stay" -- cancel mogdoor')
            and chat_has('2: "Go out" -- answer 0 (back)') and chat_has('3: "Second floor" -- turned off now')
            and chat_has('area 2: "Mog Garden" -- answer 127 (garden zone 280)')
            and chat_has('answer mogdoor <value> or cancel mogdoor (stays)')
            and chat_has('rem4line: the page waits on key, more pages follow, 0 queued, cancellable, prompt 42')
            and chat_has('  "Moogle : Welcome home,"') and chat_has('  "kupo!"')
            and chat_has('answer rem4line shows the next page, cancel rem4line ends the event')
            and chat_has('rem4li2: the page waits on timer (2.5 s left), the last page, 1 queued, no cancel, prompt 43')
            and chat_has('usage: //hideuidemo answer')
            and table.concat(args, ' ') == 'answer(mogdoor,127,41) cancel(mogdoor,41) answer(rem4line,nil,42)'
                .. ' cancel(rem4line,42)',
        'hideuidemo options prints mogdoor\'s rows and areas with the value each answers, and a dialogue box\'s'
            .. ' page, what moves it on and what follows; answer mogdoor takes an exit, answer rem4line no value: '
            .. table.concat(args, ' '))
    local all = {}
    chat = {}
    command('help')
    for _, w in ipairs({'mogdoor', 'rem4line', 'rem4li2'}) do
        local under = 0
        for _, line in ipairs({'options <name>', 'answer <name> <value>', 'cancel <name>'}) do
            for _, l in ipairs(chat) do
                if l.text:find('//hideuidemo ' .. line, 1, true) and (l.text:find(w, 1, true)
                        or (line == 'cancel <name>' and w ~= 'mogdoor' and l.text:find('dialogue boxes', 1, true)))
                        then
                    under = under + 1
                    break
                end
            end
        end
        if under == 3 then all[#all + 1] = w end
    end
    check(#all == 3, 'hideuidemo help names mogdoor and the dialogue boxes under options, answer and cancel: '
        .. table.concat(all, ' '))
end
-- the trade windows in the demo: options prints both sides; offer, offer
-- gil and withdraw reach the library, bad arguments print the usage; answer
-- takes no value; help names them
chat = {}
native_state.calls = {}
native_state.prompts = copy(prompts)
native_state.prompts.trade = {name = 'trade', id = 61, partner = 'Zaldon',
    mine = {gil = 100, items = {{slot = 1, item = 3, id = 4096, count = 2},
                               {slot = 2, item = 5, id = 4097, count = 1, pending = true}}},
    theirs = {gil = 0, items = {{slot = 1, id = 640, count = 12}}}, ok_mine = false, ok_theirs = true,
    next_change = 0.4, gray = {4, 9}}
native_state.prompts.handover = {name = 'handover', id = 62, npc = 'Moogle', gil = 300,
    items = {{slot = 1, item = 7, id = 1234, count = 1}}, next_ok = 0.7, gray = {12}}
command('options', 'trade')
command('options', 'handover')
command('offer', 'trade', '3', '2')
command('offer', 'TRADE', 'Gil', '500')
command('withdraw', 'handover', '7')
command('offer', 'trade', 'silver', '1')
command('withdraw', 'trade')
command('answer', 'trade')
command('cancel', 'handover')
do
    local args = {}
    for _, c in ipairs(native_state.calls) do
        local a = {}
        for i = 1, c.n do a[i] = tostring(c.args[i]) end
        args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
    end
    check(chat_has('trade with Zaldon, prompt 61, their OK is in') and chat_has('yours: 100 gil, 2 items')
            and chat_has('slot 1: 2 of id 4096, inventory 3')
            and chat_has('slot 2: 1 of id 4097, inventory 5 (on its way to the server)')
            and chat_has('theirs: 0 gil, 1 items') and chat_has('slot 1: 12 of id 640')
            and chat_has('the next change can go in 0.4 s') and chat_has('handover to Moogle, prompt 62: 300 gil, 1 items')
            and chat_has('grayed out by the game, cannot be offered: inventory 4, 9')
            and chat_has('the OK can go in 0.7 s')
            and chat_has('grayed out by the game, cannot be offered: inventory 12')
            and chat_has('offer trade <item> <count> | offer trade gil <amount> | withdraw trade <item>')
            and chat_has('usage: //hideuidemo offer') and chat_has('usage: //hideuidemo withdraw')
            and table.concat(args, ' ') == 'offer(trade,3,2) offer(trade,gil,500) withdraw(handover,7)'
                .. ' answer(trade,nil,61) cancel(handover,62)',
        'hideuidemo options prints the trade\'s both sides and the NPC trade\'s offer, the items the game grays out'
            .. ' and the NPC trade\'s wait; offer, offer gil, withdraw,'
            .. ' answer and cancel reach the library: ' .. table.concat(args, ' '))
    local under = {}
    chat = {}
    command('help')
    for _, line in ipairs({'options <name>', 'answer <name> <value>', 'cancel <name>', 'offer <name> <item> <count>',
                           'offer <name> gil <amount>', 'withdraw <name> <item>'}) do
        for _, l in ipairs(chat) do
            if l.text:find('//hideuidemo ' .. line, 1, true) and l.text:find('trade', 1, true)
                    and l.text:find('handover', 1, true) then
                under[#under + 1] = line
                break
            end
        end
    end
    check(#under == 6, 'hideuidemo help names trade and handover under options, answer, cancel, offer and withdraw: '
        .. table.concat(under, ', '))
    local grays = {}
    for _, l in ipairs(chat) do
        for _, line in ipairs({'options <name>', 'offer <name> <item> <count>'}) do
            if l.text:find('//hideuidemo ' .. line, 1, true) and l.text:find('grays out', 1, true) then
                grays[#grays + 1] = line
            end
        end
    end
    check(#grays == 2, 'hideuidemo help says options reads the items the game grays out and offer refuses them: '
        .. table.concat(grays, ', '))
end
native_state.prompts = prompts
native_state.prompts = prompts
chat = {}
native_state.prompts = copy(prompts)
native_state.prompts.rem4line = {name = 'rem4line', id = 44, text = 'Moogle : Welcome home,\nkupo!',
    segments = {run('Moogle : Welcome home,\nkupo!', 1)}, undecoded = 0, waits = 'key', auto = false, more = true,
    queued = 0, cancellable = true}
native_state.handles[#native_state.handles].queue = {{event = 'page', name = 'rem4line'}}
fire('prerender')
check(chat_has('page rem4line: "Moogle : Welcome home, / kupo!"'),
    'events on: hideuidemo prints a dialogue box\'s new page with its lines')
native_state.prompts = prompts

native_state.status = status_table(false, 'claim: another hideui engine holds the hooks')
chat = {}
local failed_ok, failed_err = pcall(command, 'status')
local quiet_failure = not chat_has('claim: another hideui engine')
command('debug', 'on')
chat = {}
pcall(command, 'status')
check(failed_ok and quiet_failure and chat_has('engine failed')
        and chat_has('debug: error claim: another hideui engine holds the hooks'),
    'hideuidemo status says the engine failed, and names the failure from status().detail as a debug: line only'
        .. ' with debug on' .. (failed_ok and '' or (' -- ' .. tostring(failed_err))))
native_state.status = status_table(true)
native_state.status.role = 'forwarder'
native_state.status.detail.resident = {path = 'C:\\other\\libs\\_HideUI.dll', build = '0.5.1', abi = 3}
chat = {}
command('status')
local forwarding = chat_has('debug: forwarding to the resident engine 0.5.1 from C:\\other\\libs\\_HideUI.dll')
command('debug', 'off')
chat = {}
command('status')
check(forwarding and not chat_has('forwarding to the resident') and chat_has('dropped 7'),
    'hideuidemo status names the resident it forwards to as a debug: line with debug on, and not with it off')
native_state.status = status_table(true)

chat = {}
local addon_handle = native_state.handles[#native_state.handles]
addon_handle.queue = {{event = 'blocked', name = 'menuwind', by = {'hideuidemo'}, mine = true},
                      {event = 'error', name = 'equip', verb = 'open', reason = 'the game refused it'},
                      {event = 'pending', what = 'invite', pending = true},
                      {event = 'cursor', name = 'equip', row = 3}, {event = 'cursor', name = 'query', row = 4},
                      {event = 'opened', name = 'ability', category = 2, category_name = 'pet_commands'},
                      {event = 'blocked', name = 'ability', category = 7, by = {'hideuidemo'}, mine = true},
                      {event = 'closed', name = 'ability'}}
fire('prerender')
check(chat_has('blocked menuwind (by hideuidemo, mine)') and chat_has('error: open equip: the game refused it')
        and chat_has('pending invite appeared') and chat_has('cursor equip row 3')
        and chat_has('cursor query option 4'),
    'events on: hideuidemo prints events as they arrive, who blocks, cursor moves and pending ones too, and errors'
        .. ' always')
check(chat_has('opened ability pet_commands') and chat_has('blocked ability 7 (by hideuidemo, mine)')
        and chat_has('closed ability') and not chat_has('closed ability nil'),
    'events on: hideuidemo prints the list after ability\'s name, by its name or number, and nothing when the'
        .. ' event carries none')
command('events', 'off')
chat = {}
addon_handle.queue = {{event = 'opened', name = 'equip'},
                      {event = 'error', name = 'equip', verb = 'move', reason = 'nope'},
                      {event = 'pending', what = 'post', pending = false},
                      {event = 'resync', dropped = 3}}
fire('prerender')
check(not chat_has('opened equip') and not chat_has('pending post') and chat_has('error: move equip: nope')
        and chat_has('3 events lost'),
    'events off: only errors and lost events are printed')

-- hideuidemo's text windows: options prints what each asks, answer takes
-- each window's text as typed (comment's lines split at |, link11's name
-- and colors), cancel backs out, pending lists msgline's blocked message
do
    chat = {}
    native_state.calls = {}
    native_state.prompts = copy(prompts)
    native_state.prompts.stringdl = {name = 'stringdl', id = 61, mode = 'name', min_length = 3, max_length = 15,
        prompt = 'Send to whom?'}
    native_state.prompts.comment = {name = 'comment', id = 62, mode = 'search', category = 3,
        lines = {{text = 'LFG'}, {text = ''}, {text = ''}}}
    native_state.prompts.msgline = {name = 'msgline', id = 63, mode = 0, max_length = 300, recipient = 'Friend',
        blocked = false}
    native_state.prompts.scname = {name = 'scname', id = 64, max_length = 15, text = 'Rolan'}
    native_state.prompts.link11 = {name = 'link11', id = 65, max_length = 19, text = '', color = {7, 7, 7}}
    for _, w in ipairs({'stringdl', 'comment', 'msgline', 'scname', 'link11'}) do
        command('options', w)
    end
    command('answer', 'stringdl', 'Rolanberry')
    command('answer', 'comment', 'Looking', 'for', 'party|level', '75|')
    command('answer', 'msgline', 'see', 'you')
    command('answer', 'scname', 'Rolan')
    command('answer', 'link11', 'Kupo', '1', '2', '3')
    command('answer', 'link11', 'Mog')
    command('cancel', 'stringdl')
    native_state.prompts.stringdl = {name = 'stringdl', id = 66, mode = 'confirm', prompt = 'Send it?'}
    command('options', 'stringdl')
    command('answer', 'stringdl', 'yes')
    local args = {}
    for _, c in ipairs(native_state.calls) do
        local a = {}
        for i = 1, c.n do
            local v = c.args[i]
            if type(v) == 'table' then
                v = v.name and (v.name .. (v.color and ('/' .. table.concat(v.color, ',')) or ''))
                    or ('[' .. table.concat(v, '|') .. ']')
            end
            a[i] = tostring(v)
        end
        args[#args + 1] = c.verb .. '(' .. table.concat(a, ',') .. ')'
    end
    check(chat_has('stringdl asks the recipient\'s name, 3 to 15 letters ("Send to whom?"), prompt 61')
            and chat_has('comment: the search message, category 3, prompt 62') and chat_has('line 1: "LFG"')
            and chat_has('msgline mode 0: at most 300 bytes to Friend, prompt 63')
            and chat_has('scname: a name of at most 15 letters, now "Rolan", prompt 64')
            and chat_has('link11: a name of at most 19 letters, now "", colors 7 7 7, prompt 65')
            and chat_has('stringdl asks yes or no about a delivery slot ("Send it?"), prompt 66')
            and table.concat(args, ' ') == 'answer(stringdl,Rolanberry,61) answer(comment,[Looking for party|level'
                .. ' 75|],62) answer(msgline,see you,63) answer(scname,Rolan,64) answer(link11,Kupo/1,2,3,65)'
                .. ' answer(link11,Mog,65) cancel(stringdl,61) answer(stringdl,true,66)',
        'hideuidemo options prints what each text window asks; answer takes the text as typed, comment\'s lines'
            .. ' split at |, link11\'s name and colors, stringdl yes on a slot; cancel backs out: '
            .. table.concat(args, ' '))
    chat = {}
    native_state.pending = {msgline = {name = 'msgline', id = 67, mode = 7, max_length = 15, recipient = 'Pal',
        blocked = true}}
    command('pending')
    check(chat_has('msgline mode 7: at most 15 bytes to Pal, blocked: no window, prompt 67'),
        'hideuidemo pending prints msgline\'s blocked message')
    native_state.pending = nil
    local all = {}
    chat = {}
    command('help')
    for _, w in ipairs({'stringdl', 'comment', 'msgline', 'scname', 'link11'}) do
        local under = 0
        for _, line in ipairs({'options <name>', 'answer <name> <value>', 'cancel <name>'}) do
            for _, l in ipairs(chat) do
                if l.text:find('//hideuidemo ' .. line, 1, true) and l.text:find(w, 1, true) then
                    under = under + 1
                    break
                end
            end
        end
        if under == 3 then all[#all + 1] = w end
    end
    check(#all == 5, 'hideuidemo help names each text window under options, answer and cancel: '
        .. table.concat(all, ' '))
    native_state.prompts = prompts
end

-- hideuidemo's hide and unhide take the target arrows by name, and its help
-- names them beside the compass
do
    command('hide', 'TargetArrow')
    local hidden = last_call('hide').args[1] == 'targetarrow'
    command('unhide', 'targetarrow')
    local shown = last_call('unhide').args[1] == 'targetarrow'
    chat = {}
    command('help')
    local named = false
    for _, l in ipairs(chat) do
        named = named or (l.text:find('//hideuidemo hide|unhide <name>', 1, true) ~= nil
            and l.text:find('compass and targetarrow', 1, true) ~= nil)
    end
    check(hidden and shown and named, 'hideuidemo hide and unhide targetarrow reach the engine; help names it with'
        .. ' the compass under hide|unhide')
end

-- hideuidemo with the engine down
reset_windower()
package.loaded['libs.hideui'] = nil
native_state = nil
local down_state
package.loadlib = function(path, symbol)
    return function()
        local n = make_native()
        native_state.new_error = "hideui can't work with this version of FFXI; update the addon"
        native_state.status = status_table(false, 'signature open_by_name: not found')
        down_state = native_state
        return n
    end
end
dofile(addon_dir .. '/hideuidemo.lua')
command = handlers['addon command'][1]
local down_ok = pcall(command, 'hide', 'logwindo')
check(down_ok and chat_has("[hideuidemo] hideui can't work with this version of FFXI; update the addon")
        and chat_has('hideui did not start'),
    'hideuidemo says once at load why hideui did not start, and refuses its commands')
command('debug', 'on')
down_ok = pcall(command, 'status')
command('debug', 'off')
check(down_ok and chat_has('debug: error signature open_by_name: not found') and down_state,
    'and with debug on its status still names the failure from status().detail')

print(('%d of %d checks failed'):format(failures, checks))
os.exit(failures == 0 and 0 or 1)
