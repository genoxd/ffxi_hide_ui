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
        add_to_chat = function(colour, text) chat[#chat + 1] = {colour = colour, text = text} end,
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
               detail = d}
    if ok then
        d.daemon = {abi = 1, build = '1.0.0'}
        d.registry, d.rows, d.names, d.manager = '0x020022D8', 370, 369, '0x10621838'
        d.ui = {w = 1920, h = 1080}
        s.ui = {w = 1920, h = 1080}
        d.functions = {open_by_name = '0x1', ui_update = '0x2', staged_close = '0x3', show_path = '0x4',
                       set_position = '0x5', close_by_name = '0x6'}
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

-- The engine's window names, as many as these tests use.
local windows = {'logwindo', 'logwin2', 'equip', 'menuwind', 'buff', 'ability', 'query', 'partywin', 'targetwi',
                 'subwindo', 'passinpu', 'prtyjoin', 'link5', 'arealist', 'delivery', 'post1', 'post2',
                 'playermo', 'persona'}

local native_state
local function make_native()
    native_state = {calls = {}, handles = {}, status = status_table(true), shutdown = {true}, prompts = prompts,
                    new_error = nil, results = {}, raises = {}, lists = 0}
    local native = {}
    local Handle = {}
    Handle.__index = Handle
    for _, verb in ipairs({'hide', 'unhide', 'block', 'unblock', 'move', 'move_group', 'reset', 'reset_group',
                           'reset_all', 'open', 'close', 'resize', 'answer', 'cancel'}) do
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
            all[name] = {name = name, layer = 2, open = false, blockable = name ~= 'query', detail = {}}
        end
        all.logwindo.open, all.logwindo.hidden, all.logwindo.detail.dock = true, true, 'chat_log'
        all.equip.moved = true
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

-- the game's text as UTF-8, its bytes beside it as raw; segments' colours named
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
native_state.shutdown = {false, 'the game thread did not drain in time; the engine stays installed', 'drain'}
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
native_state.shutdown = {false, 'the game thread did not drain in time; the engine stays installed', 'drain'}
fire('unload')
check(quiet and chat_has('did not drain'),
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
    {'move', 'equip', '300', '200'}, {'group', 'chat_log', '16', '830'}, {'reset', 'equip'},
    {'reset', 'equip', 'position'}, {'reset', 'equip', 'size'}, {'reset', 'equip', 'all'}, {'reset'},
    {'resetgroup', 'chat_log'}, {'resetgroup'}, {'rects'},
    {'resetall'}, {'open', 'equip'}, {'close', 'equip'}, {'info', 'equip'}, {'info', 'query'}, {'list'},
    {'list', 'log'},
    {'opened'}, {'focused'}, {'remembered'}, {'groups'}, {'info', 'arealist'}, {'resize', 'partywin', '3'},
    {'resize', 'equip', '300', '200'}, {'resize', 'partywin', 'ptw3'},
    {'options', 'query'}, {'options', 'link5'}, {'options', 'arealist'}, {'options', 'passinpu'},
    {'options', 'prtyjoin'}, {'options', 'equip'}, {'options'},
    {'answer', 'query', '4'}, {'answer', 'passinpu', 'two', 'words'}, {'answer', 'prtyjoin', 'yes'},
    {'answer', 'link5', '3'}, {'answer', 'arealist', '231'}, {'answer', 'prtyjoin', 'maybe'}, {'answer'},
    {'cancel', 'query'}, {'cancel', 'delivery'}, {'cancel'}, {'pending'}, {'answer', 'prtyjoin', 'no'},
    {'cancel', 'delivery'}, {'move', 'equip', '300', '200'}, {'group', 'chat_log', '16', '830'},
    {'layout'}, {'apply'}, {'events', 'on'}, {'debug', 'on'}, {'debug', 'off'}, {'status'}, {'help'},
    {'move', 'equip'},
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
check(chat_has('resizes by rows 1..6') and chat_has('until its owner re-sizes it') and chat_has('"Party"')
        and chat_has(' focused') and chat_has('hidden by hideuidemo, other'),
    'hideuidemo info shows the rows it resizes by, how long a size holds, focus, element text and who hides it')
check(chat_has('rect 1,2 3x4, cursor 1') and chat_has('cursor 2 of 3 (top 1)') and chat_has('cursor 3 of 3 (top 2)'),
    'hideuidemo info prints a window\'s cursor row, and query\'s and arealist\'s as the option or row under it of'
        .. ' all options(name) lists, with the first shown')
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
                      {event = 'cursor', name = 'equip', row = 3}, {event = 'cursor', name = 'query', row = 4}}
fire('prerender')
check(chat_has('blocked menuwind (by hideuidemo, mine)') and chat_has('error: open equip: the game refused it')
        and chat_has('pending invite appeared') and chat_has('cursor equip row 3')
        and chat_has('cursor query option 4'),
    'events on: hideuidemo prints events as they arrive, who blocks, cursor moves and pending ones too, and errors'
        .. ' always')
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
