-- hideui: hide, block, move, resize, open, close and answer the windows of
-- FFXI's own interface from a Windower 4 addon.
--
-- Ship hideui.lua, _HideUI.dll and hideui_daemon.dll together in the
-- addon's own libs folder.
--
--     local hideui = require('libs.hideui')
--     local ui = assert(hideui.new())
--
-- README.md documents the API.

local native
do
    local source = debug.getinfo(1, 'S').source
    local path = source:sub(1, 1) == '@' and source:sub(2) or source
    local dll = (path:gsub('hideui%.lua$', '_HideUI.dll'))

    local loader, message = package.loadlib(dll, 'luaopen__HideUI')
    if not loader then
        error('hideui: could not load ' .. dll .. ': ' .. tostring(message), 2)
    end

    native = loader()
end

local event_names = {opened = true, closed = true, covered = true, uncovered = true, blocked = true, cursor = true,
                     page = true, error = true, pending = true, resync = true}
local event_list = 'opened, closed, covered, uncovered, blocked, cursor, page, error, pending or resync'

local unnamed = {pending = true, resync = true}

-- Each group's move carries the groups after it, so apply() moves them in
-- this order.
local group_order = {'chat_log', 'party_list', 'target_window'}
local group_anchor = {chat_log = 'logwindo', party_list = 'partywin', target_window = 'targetwi'}

-- The game has two color tables; color_name() offsets the second one's
-- numbers by 256.
local color_names = {[1] = 'default', [2] = 'green'}

local released = 'this hideui handle is released'

local handles = {}
local debugging = false

local window_names

-- 123 is Windower's error color.
local function say(text)
    if not debugging then
        return
    end
    for line in tostring(text):gmatch('[^\n]+') do
        windower.add_to_chat(123, '[hideui] ' .. line)
    end
end

local function copy(t)
    if type(t) ~= 'table' then
        return t
    end
    local out = {}
    for key, value in pairs(t) do
        out[key] = copy(value)
    end
    return out
end

local function pack(...)
    return {n = select('#', ...), ...}
end

local function public(result)
    if debugging or type(result) ~= 'table' then
        return result
    end
    result.detail = nil
    for _, value in pairs(result) do
        if type(value) == 'table' then
            value.detail = nil
        end
    end
    return result
end

local function utf8(text)
    local convert = windower.from_shift_jis
    if type(text) ~= 'string' or type(convert) ~= 'function' then
        return text
    end
    local ok, converted = pcall(convert, text)
    return ok and type(converted) == 'string' and converted or text
end

local function convert_field(t, key)
    if type(t) == 'table' and type(t[key]) == 'string' then
        t.raw = t[key]
        t[key] = utf8(t[key])
    end
end

local function color_name(segment)
    local n = segment.color
    if type(n) ~= 'number' then
        return n
    end
    if segment.escape == 0x1F then
        n = n + 0x100
    end
    return color_names[n] or ('color' .. n)
end

local function convert_glyph_text(t)
    if type(t) ~= 'table' then
        return
    end
    convert_field(t, 'text')
    for _, segment in ipairs(type(t.segments) == 'table' and t.segments or {}) do
        segment.color = color_name(segment)
        segment.escape = nil
        convert_field(segment, 'text')
    end
end

local function convert_prompt(o)
    if type(o) == 'table' then
        if type(o.text) == 'string' then
            convert_glyph_text(o)
        end
        convert_glyph_text(o.title)
        for _, option in ipairs(type(o.options) == 'table' and o.options or {}) do
            convert_glyph_text(option)
        end
        for _, area in ipairs(type(o.areas) == 'table' and o.areas or {}) do
            convert_glyph_text(area)
        end
        for _, slot in ipairs(type(o.slots) == 'table' and o.slots or {}) do
            convert_field(slot, 'name')
        end
        for _, row in ipairs(type(o.rows) == 'table' and o.rows or {}) do
            convert_field(row, 'text')
            if type(row.label) == 'string' then
                row.label_raw = row.label
                row.label = utf8(row.label)
            end
        end
        for _, line in ipairs(type(o.lines) == 'table' and o.lines or {}) do
            convert_field(line, 'text')
        end
        for _, key in ipairs({'prompt', 'recipient', 'partner', 'npc'}) do
            if type(o[key]) == 'string' then
                o[key .. '_raw'] = o[key]
                o[key] = utf8(o[key])
            end
        end
        convert_field(o, 'inviter')
        convert_field(o, 'gm')
    end
    return o
end

local function convert_pending(p)
    if type(p) == 'table' then
        for _, entry in pairs(p) do
            convert_prompt(entry)
        end
    end
    return p
end

local function convert_info(i)
    if type(i) == 'table' then
        for _, element in ipairs(type(i.elements) == 'table' and i.elements or {}) do
            convert_field(element, 'text')
        end
    end
    return i
end

local function convert_settings(list)
    for _, setting in ipairs(type(list) == 'table' and list or {}) do
        if type(setting.label) == 'string' then
            setting.label_raw = setting.label
            setting.label = utf8(setting.label)
        end
    end
    return list
end

local converters = {options = convert_prompt, pending = convert_pending, info = convert_info,
                    settings = convert_settings}

local function read_result(verb, result, ...)
    local convert = converters[verb]
    result = public(result)
    if convert then
        result = convert(result)
    end
    return result, ...
end

local Handle = {}
Handle.__index = Handle

-- Level 3 raises at the addon's line, the caller of the method calling this.
local function need(ok, message)
    if not ok then
        error(message, 3)
    end
end

local function check_self(self, verb)
    if getmetatable(self) ~= Handle then
        error(('use ui:%s(...)'):format(verb), 3)
    end
end

-- The engine raises misuse only it can see, such as an answer of the wrong
-- type for its window; raise it again at the addon's line.
local function call(self, verb, ...)
    local results = pack(pcall(self.native[verb], self.native, ...))
    if not results[1] then
        error(results[2], 3)
    end
    return unpack(results, 2, results.n)
end

local function sorted_keys(t)
    local keys = {}
    for key in pairs(t) do
        keys[#keys + 1] = key
    end
    table.sort(keys, function(a, b) return tostring(a) < tostring(b) end)
    return keys
end

-- The first name, which events carry, or nil for no such window. Only the
-- engine knows the second names, so info() resolves them. Without the
-- engine's list, the name lowercased.
local function first_name(self, name)
    local low = name:lower()
    if not window_names and self.native then
        local all = self.native:list()
        if type(all) == 'table' then
            window_names = {}
            for key in pairs(all) do
                window_names[key] = key
            end
        end
    end
    if not window_names then
        return low
    end
    if not window_names[low] and self.native then
        local i = self.native:info(low)
        if type(i) == 'table' and type(i.name) == 'string' and window_names[i.name] then
            window_names[low] = i.name
        end
    end
    return window_names[low]
end

local function layout_key(self, name)
    return first_name(self, name) or name:lower()
end

local function no_entries()
    return {groups = {}, positions = {}, sizes = {}}
end

-- A group move is its anchor's position, so a position reset of the anchor
-- forgets it too.
local function forget(self, name, aspect)
    name = layout_key(self, name)
    for _, entries in ipairs({self.intent, self.kept}) do
        if aspect ~= 'size' then
            entries.positions[name] = nil
            for group, anchor in pairs(group_anchor) do
                if anchor == name then
                    entries.groups[group] = nil
                end
            end
        end
        if aspect ~= 'position' then
            entries.sizes[name] = nil
        end
    end
end

local function note_ui(self)
    local s = native.status()
    if type(s) == 'table' and type(s.ui) == 'table' and type(s.ui.w) == 'number' and type(s.ui.h) == 'number' then
        self.intent.ui = {w = s.ui.w, h = s.ui.h}
    end
end

-- A group move places its anchor and every group nested in it, so their
-- own entries no longer hold.
local function record_group(self, group, x, y)
    group = group:lower()
    local intent = self.intent
    intent.groups[group] = {x = x, y = y}
    intent.positions[group_anchor[group] or ''] = nil
    local all = self.native and self.native:groups()
    local moved = type(all) == 'table' and type(all[group]) == 'table' and all[group].members
    if type(moved) ~= 'table' then
        return
    end
    local members = {}
    for _, name in ipairs(moved) do
        members[name] = true
    end
    for other, g in pairs(all) do
        if other ~= group and type(g) == 'table' and members[g.anchor] then
            intent.groups[other] = nil
            intent.positions[g.anchor] = nil
        end
    end
end

local function record_size(self, key, a, b)
    self.intent.sizes[key] = b and {w = a, h = b} or {rows = a}
end

local function window_call(verb, usage, reads)
    return function(self, name)
        check_self(self, verb)
        need(type(name) == 'string', usage .. ': name must be a string')
        if not self.native then
            return nil, released
        end
        if reads then
            return read_result(verb, call(self, verb, name))
        end
        return call(self, verb, name)
    end
end

for _, verb in ipairs({'hide', 'unhide', 'close'}) do
    Handle[verb] = window_call(verb, verb .. '(name)')
end

-- The engine takes an argument as one string, a table as key=value pairs
-- joined by ';', so a string may hold neither '=' nor ';'. %.17g keeps a
-- fraction exact; the engine decides which windows take one.
local function argument_word(value, where)
    if type(value) == 'number' then
        if value ~= value or value == math.huge or value == -math.huge then
            return nil, where .. ' must be a finite number'
        end
        if value == math.floor(value) and math.abs(value) < 2^31 then
            return string.format('%d', value)
        end
        return string.format('%.17g', value)
    end
    if type(value) ~= 'string' then
        return nil, where .. ' must be a string or a number'
    end
    if value == '' or value:find('[=;]') then
        return nil, where .. ' must be a non-empty string without = or ;'
    end
    return value
end

local function argument_text(argument)
    if type(argument) ~= 'table' then
        return argument_word(argument, 'the argument')
    end
    local keys = {}
    for key in pairs(argument) do
        if type(key) ~= 'string' or not key:find('^[%a_][%w_]*$') then
            return nil, 'a table argument takes named fields only'
        end
        keys[#keys + 1] = key
    end
    if #keys == 0 then
        return nil, 'a table argument needs a field'
    end
    table.sort(keys)
    local fields = {}
    for _, key in ipairs(keys) do
        local word, why = argument_word(argument[key], 'field ' .. key)
        if not word then
            return nil, why
        end
        fields[#fields + 1] = key .. '=' .. word
    end
    return table.concat(fields, ';')
end

local function argument_call(verb, what)
    local usage = verb .. '(' .. what .. '[, argument])'
    return function(self, name, argument)
        check_self(self, verb)
        need(type(name) == 'string', usage .. ': ' .. what .. ' must be a string')
        local text, why
        if argument ~= nil then
            text, why = argument_text(argument)
            need(text ~= nil, usage .. ': ' .. tostring(why))
        end
        if not self.native then
            return nil, released
        end
        if text == nil then
            return call(self, verb, name)
        end
        return call(self, verb, name, text)
    end
end

Handle.open = argument_call('open', 'name')
Handle.open_set = argument_call('open_set', 'set')

-- The category reaches the engine only when given, so an older resident
-- engine still serves the plain call.
local function category_call(verb)
    local usage = verb .. '(name[, category])'
    return function(self, name, category)
        check_self(self, verb)
        need(type(name) == 'string', usage .. ': name must be a string')
        need(category == nil or type(category) == 'string', usage .. ': category must be a string')
        if not self.native then
            return nil, released
        end
        if category == nil then
            return call(self, verb, name)
        end
        return call(self, verb, name, category)
    end
end

for _, verb in ipairs({'block', 'unblock'}) do
    Handle[verb] = category_call(verb)
end

for _, verb in ipairs({'info', 'options'}) do
    Handle[verb] = window_call(verb, verb .. '(name)', true)
end

local function read_call(verb)
    return function(self)
        check_self(self, verb)
        if not self.native then
            return nil, released
        end
        return read_result(verb, call(self, verb))
    end
end

for _, verb in ipairs({'list', 'opened', 'focused', 'remembered', 'groups', 'pending', 'rects', 'macros',
                       'settings'}) do
    Handle[verb] = read_call(verb)
end

function Handle:setting(name)
    check_self(self, 'setting')
    need(type(name) == 'string', 'setting(name): name must be a string')
    if not self.native then
        return nil, released
    end
    return call(self, 'setting', name)
end

-- The engine checks value against the setting's kind.
function Handle:set_setting(name, value)
    check_self(self, 'set_setting')
    need(type(name) == 'string', 'set_setting(name, value): name must be a string')
    if not self.native then
        return nil, released
    end
    return call(self, 'set_setting', name, value)
end

local function plain_call(verb)
    return function(self)
        check_self(self, verb)
        if not self.native then
            return nil, released
        end
        return call(self, verb)
    end
end

for _, verb in ipairs({'block_macros', 'unblock_macros'}) do
    Handle[verb] = plain_call(verb)
end

function Handle:move(name, x, y)
    check_self(self, 'move')
    need(type(name) == 'string', 'move(name, x, y): name must be a string')
    need(type(x) == 'number' and type(y) == 'number', 'move(name, x, y): x and y must be numbers')
    if not self.native then
        return nil, released
    end
    local ok, why = call(self, 'move', name, x, y)
    if ok then
        self.intent.positions[layout_key(self, name)] = {x = x, y = y}
        note_ui(self)
    end
    return ok, why
end

function Handle:move_group(group, x, y)
    check_self(self, 'move_group')
    need(type(group) == 'string', 'move_group(group, x, y): group must be a string')
    need(type(x) == 'number' and type(y) == 'number', 'move_group(group, x, y): x and y must be numbers')
    if not self.native then
        return nil, released
    end
    local ok, why = call(self, 'move_group', group, x, y)
    if ok then
        record_group(self, group, x, y)
        note_ui(self)
    end
    return ok, why
end

function Handle:resize(name, a, b)
    check_self(self, 'resize')
    need(type(name) == 'string', 'resize(name, rows) or resize(name, w, h): name must be a string')
    need(type(a) == 'number' and (b == nil or type(b) == 'number'),
        'resize(name, rows) or resize(name, w, h): rows, w and h must be numbers')
    if not self.native then
        return nil, released
    end
    local ok, why = call(self, 'resize', name, a, b)
    if ok then
        record_size(self, layout_key(self, name), a, b)
        note_ui(self)
    end
    return ok, why
end

function Handle:reset(name, aspect)
    check_self(self, 'reset')
    need(type(name) == 'string', 'reset(name[, aspect]): name must be a string')
    need(aspect == nil or aspect == 'position' or aspect == 'size',
        "reset(name, aspect): aspect must be 'position' or 'size'")
    if not self.native then
        return nil, released
    end
    local ok, why = call(self, 'reset', name, aspect)
    if ok then
        forget(self, name, aspect)
        note_ui(self)
    end
    return ok, why
end

function Handle:reset_group(group)
    check_self(self, 'reset_group')
    need(type(group) == 'string', 'reset_group(group): group must be a string')
    if not self.native then
        return nil, released
    end
    local ok, why = call(self, 'reset_group', group)
    if ok then
        self.intent.groups[group:lower()] = nil
        self.kept.groups[group:lower()] = nil
        note_ui(self)
    end
    return ok, why
end

function Handle:reset_all()
    check_self(self, 'reset_all')
    if not self.native then
        return nil, released
    end
    local ok, why = call(self, 'reset_all')
    if ok then
        self.intent = no_entries()
        self.kept = no_entries()
        note_ui(self)
    end
    return ok, why
end

local function reply_target(usage, id_usage, prompt, id)
    if type(prompt) == 'table' then
        if type(prompt.name) ~= 'string' then
            error(('%s: the prompt must be the table options() or pending() returned'):format(usage), 3)
        end
        if id ~= nil then
            error(('%s: a prompt table carries its own id'):format(usage), 3)
        end
        prompt, id = prompt.name, prompt.id
    end
    if type(prompt) ~= 'string' then
        error(('%s: name must be a string, or the table options() or pending() returned'):format(usage), 3)
    end
    if id ~= nil and type(id) ~= 'number' then
        error(id_usage .. ': id must be the number options(name) gave', 3)
    end
    return prompt, id
end

-- The engine checks value against the window.
function Handle:answer(prompt, value, id)
    check_self(self, 'answer')
    local name
    name, id = reply_target('answer(prompt, value) or answer(name, value[, id])', 'answer(name, value, id)', prompt, id)
    if not self.native then
        return nil, released
    end
    return call(self, 'answer', name, value, id)
end

function Handle:cancel(prompt, id)
    check_self(self, 'cancel')
    local name
    name, id = reply_target('cancel(prompt) or cancel(name[, id])', 'cancel(name, id)', prompt, id)
    if not self.native then
        return nil, released
    end
    return call(self, 'cancel', name, id)
end

function Handle:offer(name, item, count)
    check_self(self, 'offer')
    need(type(name) == 'string', 'offer(name, item, count): name must be a string')
    need(type(item) == 'number' or item == 'gil', "offer(name, item, count): item must be an inventory index or 'gil'")
    need(type(count) == 'number', 'offer(name, item, count): count must be a number')
    if not self.native then
        return nil, released
    end
    return call(self, 'offer', name, item, count)
end

function Handle:withdraw(name, item)
    check_self(self, 'withdraw')
    need(type(name) == 'string', 'withdraw(name, item): name must be a string')
    need(type(item) == 'number', 'withdraw(name, item): item must be an inventory index')
    if not self.native then
        return nil, released
    end
    return call(self, 'withdraw', name, item)
end

-- Answers on a released handle too: this file's unload handler releases the
-- handles before the addon's own handler saves the layout. A fresh copy
-- each call, so the caller may keep or change it.
function Handle:layout()
    check_self(self, 'layout')
    local layout = {}
    for _, section in ipairs({'groups', 'positions', 'sizes'}) do
        local t = {}
        for name, entry in pairs(self.kept[section]) do
            t[name] = copy(entry)
        end
        for name, entry in pairs(self.intent[section]) do
            t[name] = copy(entry)
        end
        if next(t) ~= nil then
            layout[section] = t
        end
    end
    if self.intent.ui then
        layout.ui = copy(self.intent.ui)
    end
    return layout
end

local function section(layout, key)
    local t = layout[key]
    if t == nil then
        return {}
    end
    if type(t) ~= 'table' then
        error(('apply(layout): layout.%s must be a table'):format(key), 3)
    end
    return t
end

local function entry(t, key, where)
    local e = t[key]
    if type(e) ~= 'table' then
        error(('apply(layout): %s.%s must be a table'):format(where, tostring(key)), 3)
    end
    return e
end

function Handle:apply(layout)
    check_self(self, 'apply')
    if type(layout) ~= 'table' then
        error('apply(layout) needs the table layout() returned', 2)
    end
    local saved_ui = layout.ui
    if saved_ui ~= nil and (type(saved_ui) ~= 'table' or type(saved_ui.w) ~= 'number'
            or type(saved_ui.h) ~= 'number') then
        error('apply(layout): layout.ui needs numbers w and h', 2)
    end
    local groups, positions, sizes = section(layout, 'groups'), section(layout, 'positions'),
        section(layout, 'sizes')
    for _, name in ipairs(sorted_keys(groups)) do
        local p = entry(groups, name, 'groups')
        if type(p.x) ~= 'number' or type(p.y) ~= 'number' then
            error(('apply(layout): groups.%s needs numbers x and y'):format(tostring(name)), 2)
        end
    end
    for _, name in ipairs(sorted_keys(positions)) do
        local p = entry(positions, name, 'positions')
        if type(p.x) ~= 'number' or type(p.y) ~= 'number' then
            error(('apply(layout): positions.%s needs numbers x and y'):format(tostring(name)), 2)
        end
    end
    for _, name in ipairs(sorted_keys(sizes)) do
        local z = entry(sizes, name, 'sizes')
        if type(z.rows) ~= 'number' and (type(z.w) ~= 'number' or type(z.h) ~= 'number') then
            error(('apply(layout): sizes.%s needs a number rows, or numbers w and h'):format(tostring(name)), 2)
        end
    end
    if not self.native then
        return nil, released
    end
    if saved_ui then
        local s = native.status()
        local now = type(s) == 'table' and s.ui
        if type(now) == 'table' and (now.w ~= saved_ui.w or now.h ~= saved_ui.h) then
            return nil, ('this layout was saved at %dx%d; the game is at %dx%d'):format(saved_ui.w, saved_ui.h,
                now.w, now.h)
        end
    end

    local failed = {}
    local applied = false
    local function try(where, verb, name, kept, ok, why)
        local key = where == 'groups' and tostring(name):lower() or layout_key(self, tostring(name))
        if ok then
            self.kept[where][key] = nil
            applied = true
            if where == 'groups' then
                record_group(self, key, kept.x, kept.y)
            elseif where == 'positions' then
                self.intent.positions[key] = {x = kept.x, y = kept.y}
            else
                record_size(self, key, kept.rows or kept.w, kept.h)
            end
        else
            failed[#failed + 1] = {verb = verb, name = tostring(name), reason = tostring(why)}
            self.kept[where][key] = kept
        end
    end
    local done = {}
    for _, name in ipairs(group_order) do
        local p = groups[name]
        if p then
            done[name] = true
            try('groups', 'move_group', name, {x = p.x, y = p.y}, self.native:move_group(name, p.x, p.y))
        end
    end
    for _, name in ipairs(sorted_keys(groups)) do
        if not done[name] then
            local p = groups[name]
            try('groups', 'move_group', name, {x = p.x, y = p.y}, nil,
                'no such group (chat_log, party_list or target_window)')
        end
    end
    for _, name in ipairs(sorted_keys(positions)) do
        local p = positions[name]
        try('positions', 'move', name, {x = p.x, y = p.y}, self.native:move(tostring(name), p.x, p.y))
    end
    for _, name in ipairs(sorted_keys(sizes)) do
        local z = sizes[name]
        if type(z.rows) == 'number' then
            try('sizes', 'resize', name, {rows = z.rows}, self.native:resize(tostring(name), z.rows))
        else
            try('sizes', 'resize', name, {w = z.w, h = z.h}, self.native:resize(tostring(name), z.w, z.h))
        end
    end
    if applied then
        note_ui(self)
    end
    if #failed > 0 then
        local lines = {}
        for i, f in ipairs(failed) do
            lines[i] = ('%s %s: %s'):format(f.verb, f.name, f.reason)
        end
        return nil, table.concat(lines, '; '), failed
    end
    return true
end

local function check_event(event)
    if not event_names[event] then
        error(('no such event: %s (%s)'):format(tostring(event), event_list), 3)
    end
end

-- Returns the first name: events carry it, so a second name must match it.
local function check_window(self, verb, event, name)
    if name == nil then
        return nil
    end
    if type(name) ~= 'string' then
        error(('%s(event, name, fn): name must be a window name'):format(verb), 3)
    end
    if unnamed[event] then
        error(('%s(event, name, fn): %s events name no window'):format(verb, event), 3)
    end
    local first = first_name(self, name)
    if not first then
        error(('%s(event, name, fn): no such window: %s'):format(verb, name), 3)
    end
    return first
end

-- on() and off() take (event, fn), (event, name, fn) or (event, name,
-- category, fn), off() each without fn too: the strings given are the name
-- and the category, in that order.
local function listener_args(name, category, fn)
    if fn == nil then
        if category == nil and type(name) ~= 'string' then
            return nil, nil, name
        end
        if type(category) ~= 'string' then
            return name, nil, category
        end
    end
    return name, category, fn
end

local function check_category(verb, name, category)
    if category == nil then
        return
    end
    if type(category) ~= 'string' then
        error(('%s(event, name, category, fn): category must be a string'):format(verb), 3)
    end
    if type(name) ~= 'string' or name:lower() ~= 'ability' then
        error('category filters apply to ability only', 3)
    end
end

function Handle:on(event, name, category, fn)
    check_self(self, 'on')
    name, category, fn = listener_args(name, category, fn)
    check_event(event)
    local window = check_window(self, 'on', event, name)
    check_category('on', name, category)
    if type(fn) ~= 'function' then
        error('on(event[, name[, category]], fn) needs a function', 2)
    end
    local list = self.callbacks[event]
    list[#list + 1] = {fn = fn, name = window, category = category and category:lower()}
    return fn
end

function Handle:off(event, name, category, fn)
    check_self(self, 'off')
    name, category, fn = listener_args(name, category, fn)
    check_event(event)
    local window = check_window(self, 'off', event, name)
    check_category('off', name, category)
    if fn ~= nil and type(fn) ~= 'function' then
        error('off(event[, name[, category]], fn): fn must be a function', 2)
    end
    local list = self.callbacks[event]
    local wanted = category and category:lower()
    for i = #list, 1, -1 do
        local c = list[i]
        if (window == nil or c.name == window) and (wanted == nil or c.category == wanted)
                and (fn == nil or c.fn == fn) then
            c.removed = true
            table.remove(list, i)
        end
    end
    return true
end

function Handle:status()
    check_self(self, 'status')
    local s, why = native.status()
    if not s then
        return nil, why
    end
    s.dropped = self.dropped
    if type(s.detail) == 'table' then
        s.detail.callback_errors = self.callback_errors
    end
    return public(s)
end

function Handle:release()
    check_self(self, 'release')
    if not self.native then
        return true
    end
    local native_handle = self.native
    self.native = nil
    for i = #handles, 1, -1 do
        if handles[i] == self then
            table.remove(handles, i)
        end
    end
    return native_handle:release()
end

local hideui = {}

function hideui.new(name)
    if name == nil and type(_addon) == 'table' then
        name = _addon.name
    end
    if type(name) ~= 'string' then
        error('hideui.new([name]): name must be a string, or _addon.name set', 2)
    end
    local native_handle, why = native.new(name)
    if not native_handle then
        say(name .. ': ' .. tostring(why))
        local s = native.status()
        if s and type(s.detail) == 'table' and s.detail.error then
            say(name .. ': ' .. tostring(s.detail.error))
        end
        return nil, why
    end

    local callbacks = {}
    for event in pairs(event_names) do
        callbacks[event] = {}
    end
    local handle = setmetatable({
        name = name,
        native = native_handle,
        callbacks = callbacks,
        intent = no_entries(),
        kept = no_entries(),
        dropped = 0,
        callback_errors = 0,
    }, Handle)
    handles[#handles + 1] = handle
    return handle
end

-- callback_errors counts this addon's handles only: each addon runs its own
-- copy of this file.
function hideui.status()
    local s, why = native.status()
    if not s then
        return nil, why
    end
    if type(s.detail) == 'table' then
        local errors = 0
        for _, handle in ipairs(handles) do
            errors = errors + handle.callback_errors
        end
        s.detail.callback_errors = errors
    end
    return public(s)
end

function hideui.version()
    return native.version()
end

function hideui.debug(on)
    if type(on) ~= 'boolean' then
        error('hideui.debug(on): on must be true or false', 2)
    end
    debugging = on
end

local function category_matches(c, event)
    if c.category == nil or event.category_name == c.category then
        return true
    end
    local number = tonumber(c.category)
    return number ~= nil and event.category == number
end

-- A failing error callback is only counted: posting it as another error
-- event would loop.
local function deliver(handle, event)
    local list = handle.callbacks[event.event]
    if not list or #list == 0 then
        return
    end
    local snapshot = {unpack(list)}
    for i = 1, #snapshot do
        local c = snapshot[i]
        if not handle.native then
            return
        end
        if not c.removed and (c.name == nil or c.name == event.name) and category_matches(c, event) then
            local ok, message = pcall(c.fn, event)
            if not ok then
                handle.callback_errors = handle.callback_errors + 1
                if not c.failed then
                    c.failed = true
                    say(('%s: %s callback failed: %s'):format(handle.name, event.event, tostring(message)))
                    if event.event ~= 'error' then
                        deliver(handle, {event = 'error', verb = 'callback', name = event.name or '',
                                         reason = event.event .. ' callback: ' .. tostring(message)})
                    end
                end
            end
        end
    end
end

windower.register_event('prerender', function()
    for i = #handles, 1, -1 do
        local handle = handles[i]
        if handle and handle.native then
            local list, dropped = handle.native:poll()
            if list then
                handle.dropped = handle.dropped + (dropped or 0)
                for j = 1, #list do
                    if not handle.native then
                        break
                    end
                    local event = list[j]
                    if event.event == 'error' then
                        say(('%s: %s %s: %s'):format(handle.name, tostring(event.verb), tostring(event.name),
                            tostring(event.reason)))
                    elseif event.event == 'resync' then
                        say(('%s: %s events lost'):format(handle.name, tostring(event.dropped)))
                    end
                    deliver(handle, event)
                end
            end
        end
    end
end)

windower.register_event('unload', function()
    for i = #handles, 1, -1 do
        pcall(handles[i].release, handles[i])
    end
    -- Another addon still holding handles is the normal refusal.
    local ok, why, kind = native.shutdown()
    if not ok and kind ~= 'handles' then
        say('shutdown: ' .. tostring(why))
    end
end)

return hideui
