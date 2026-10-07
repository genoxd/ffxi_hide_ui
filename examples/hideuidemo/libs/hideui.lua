--[[
    hideui - hide, block, move, resize, open, close and answer the windows of FFXI's own interface.

        local hideui = require('libs.hideui')
        local ui = assert(hideui.new())      -- named after _addon.name

        ui:hide('logwindo')                  -- invisible, still open, takes no input
        ui:move('equip', 300, 200)
        ui:on('opened', 'equip', function(event) end)

    Ship hideui.lua, _HideUI.dll and hideui_daemon.dll together in the addon's
    libs folder.

    hideui.new([name]) returns a handle, or nil and the reason when the engine
    cannot work; with no name it takes _addon.name. Every verb returns true,
    or nil and the reason; every read returns its table, or nil and the
    reason. A call with an argument of the wrong type, or without one it
    needs, raises at the addon's line, and so does ui.hide(...) written for
    ui:hide(...). Changes reach the game on its
    next frame; when the game thread then refuses one, the handle that made
    the call gets an `error` event.

    This file polls events every frame and releases the handles on unload --
    dropping their hides and blocks and resetting the windows they moved or
    resized last. It prints nothing unless hideui.debug(true), and only then
    do status(), info(), list() and pending() carry the engine's internals
    under `detail`.

    Events, each a table with .event:
        opened     the game opened the window, an open of a
                   window already open included
        closed     the window left the screen and is gone
        covered    another window went over it
        uncovered  it came back from under that window
        blocked    an open was refused because of a block; every handle gets
                   it, whichever holds the block. .by lists the handles
                   holding a block on it, .mine whether this one is one
        cursor     the cursor of an open window moved to .row; for query
                   and arealist, .row is the option or row under it, its
                   index in options('query').options or
                   options('arealist').rows, which moves when the list
                   scrolls though the row on screen does not
        error      the game thread refused a call this handle queued:
                   .verb, .name and .reason. A group move waiting for its
                   anchor that another handle's move_group replaced is one,
                   .verb 'move_group'. A callback of this handle that fails
                   is posted once as an error too, .verb 'callback'.
        pending    a party invite or a post-box session appeared or cleared:
                   .what 'invite' or 'post', .pending true or false
        resync     events were lost, .dropped of them, and the rest of this
                   frame's came before it: read the state again
    All but pending and resync name their window in .name; on(event, name,
    fn) takes a window name for those alone. On ability, opened, closed,
    covered, uncovered and blocked carry .category, the list the window
    shows or was asked for (1 job_abilities, 2 pet_commands, 3
    weapon_skills, 4 job_traits), and .category_name for the ones the
    engine names; on(event, 'ability', category, fn) takes one of those
    names or a number, and off(event, 'ability', category[, fn]) the same.
    Events start at new(): sync with ui:opened(), ui:info() and ui:pending()
    after it.

    A hidden window takes no input of any kind: no click lands on it, and
    none of the player's keys or gamepad buttons reach it. The addon owns
    whatever it draws in its place: it reads keys through Windower and
    replies with answer and cancel. Escape does not close a hidden prompt
    either, so an addon that hides one must answer or cancel it. Typed
    text goes to the game's text entry rather than to a window, so a hidden
    passinpu's field may still take typed characters; Enter and Escape do
    not reach it.

    info(name).cursor is the open window's cursor row; info('query').cursor
    is the option under the cursor, as the cursor event's .row, and .top the
    first option shown (both 1-based in options('query').options), so a list
    drawn three rows at a time scrolls as the game's does. info('arealist')
    is the same, 1-based in options('arealist').rows.

    block(name) refuses the window's opens and hides it, and closes it
    through the game's own close when it is open (not the prompt windows
    close() refuses). block('ability', category) refuses one list of the
    ability window alone, the others opening as before, and closes the
    window when it shows that list; unblock('ability', category) drops
    that hold, and unblock('ability') the whole-window one alone.
    info('ability').blocked_categories lists the lists any handle blocks,
    .mine.blocked_categories this handle's. move and move_group take the
    frame's top-left,
    info(name).rect's x and y, for every window. reset(name[, 'position' |
    'size']) and reset_group(group) reset only what this handle placed, and
    refuse, naming who placed it, when another handle did.

    block_macros() stops the game's macro keys, Ctrl+number and
    Alt+number, and closes a macro bar that is up; unblock_macros() drops
    this handle's hold, and the keys stay blocked while another handle
    holds one. macros() is {blocked, blocked_by, mine}: whether they are
    blocked, the handles holding a block, and whether this one is one;
    hideui.status().macros_blocked says whether any does. The macro keys
    are no window: no name reaches them and no event names them.

    answer and cancel take the table options() or pending() returned (its
    name and id) in place of the window's name. Text the game shows comes
    back as UTF-8 in .text (or .name, .inviter), its Shift-JIS bytes in
    .raw beside it (an arealist row's .label has its own in .label_raw); a
    segment's .color is 'default', 'green' or 'color<n>'.

    options('arealist') is {name, id, mode, level, pending, rows}: every
    row of the list in order, each {text, id, kind} with .label, the text
    beside it, or .count, a region's zones. kind is 'current_area',
    'current_region', 'all', 'region' or 'zone' ('other' in mode 4's list);
    level is 0 at the top and a region's -id inside it; pending is true: the
    list waits on a choice. answer('arealist', id) and cancel('arealist')
    reply to an NPC's prompt (mode 1 or 2) and end it as the game's own
    Enter on a zone and its cancel do: answer gives the NPC the zone, cancel
    leaves it unanswered, and the list closes. With the list open, id is a
    zone row's id. With it blocked, options() still answers while the NPC
    waits, with rows = {}, and any zone id, 0..511, goes. block('arealist')
    leaves the NPC waiting for that answer or cancel. The search menu's list
    (mode 0), and modes 3 and 4, are read-only through the library: the
    player's keys drive them, and only while they are not hidden.

    cancel('query') writes the choice's cancel answer, as the game's own
    cancel does, while the event allows one; the NPC's script then closes query
    itself. cancel('post1') or cancel('post2') with the incoming box up is
    the box's own cancel: it asks the server to end the session, and the
    boxes close when the server answers. cancel('delivery') with the
    outgoing box up is its closing row: the game takes back what was staged
    and asks the server, and the box closes when the server answers. With a
    box blocked, cancel sends the same request with no window to take
    down; the session ends when the server replies, and with the outgoing
    box the player can move again only then. closed events follow when the
    game takes the windows down, and the pending event when the session
    ends. The incoming box refuses a cancel while it is closing, under
    another window or not yet taking input; the outgoing box while it is
    closing, under another window or busy.

    layout() is what this handle asked for -- each move, move_group,
    resize and reset of it, and the entries apply() could not apply -- with
    the game's UI size at the last change as .ui, in a shape Windower's
    config library saves as it is. It answers at once, whatever other
    addons did since, and the same after the handle is released (this
    file's unload handler runs before the addon's own). apply(layout)
    replays it, and refuses a layout saved at another UI size.
]]

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
                     error = true, pending = true, resync = true}
local event_list = 'opened, closed, covered, uncovered, blocked, cursor, error, pending or resync'

-- The events that name no window.
local unnamed = {pending = true, resync = true}

-- The order apply() moves groups in: the chat log's move carries the party
-- list's group, so that one goes after it.
local group_order = {'chat_log', 'party_list', 'target_window'}
local group_anchor = {chat_log = 'logwindo', party_list = 'partywin', target_window = 'targetwi'}

-- The group whose anchor a group's move places too.
local group_carries = {chat_log = 'party_list'}

-- Color names for the two color tables a row can use; the second table's
-- indices are offset by 256.
local color_names = {[1] = 'default', [2] = 'green'}

local released = 'this hideui handle is released'

local handles = {}
local debugging = false

-- The engine's window names, read once through a live handle.
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

-- The engine's internals, its own failure text included, sit under `detail`,
-- in a result and in each table one level into it, and reach the addon only
-- with hideui.debug(true).
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

-- The game's text, Shift-JIS, as UTF-8 to draw. Without Windower's
-- converter the bytes stay as they are.
local function utf8(text)
    local convert = windower.from_shift_jis
    if type(text) ~= 'string' or type(convert) ~= 'function' then
        return text
    end
    local ok, converted = pcall(convert, text)
    return ok and type(converted) == 'string' and converted or text
end

-- t[key] as UTF-8, its bytes as t.raw.
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

-- {text, segments, undecoded}: the text and each segment's converted, each
-- segment's color named.
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
        convert_glyph_text(o.title)
        for _, option in ipairs(type(o.options) == 'table' and o.options or {}) do
            convert_glyph_text(option)
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
        convert_field(o, 'inviter')
    end
    return o
end

local function convert_pending(p)
    if type(p) == 'table' then
        convert_field(p.invite, 'inviter')
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

local converters = {options = convert_prompt, pending = convert_pending, info = convert_info}

-- A read's result through public() and, for a read of the game's text, its
-- converter; the reason after it untouched.
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

-- Misuse, raised at the line that called the method calling this.
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

-- The native call. Misuse only the engine can see -- an answer of the wrong
-- type for its window -- raises there, and is raised again at the addon's
-- line.
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

-- A handle's layout entries by section: what it asked for (`intent`), and
-- the entries apply() could not apply (`kept`), which layout() keeps until
-- the window is reset or the same entry is applied.
local function no_entries()
    return {groups = {}, positions = {}, sizes = {}}
end

-- What a reset of window `name` forgets, of `aspect` or both: its entries,
-- and with its position a group move whose anchor it is.
local function forget(self, name, aspect)
    name = name:lower()
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

-- The game's UI size as the layout records it at each change.
local function note_ui(self)
    local s = native.status()
    if type(s) == 'table' and type(s.ui) == 'table' and type(s.ui.w) == 'number' and type(s.ui.h) == 'number' then
        self.intent.ui = {w = s.ui.w, h = s.ui.h}
    end
end

-- A move_group places its anchor, and the anchor of the group it carries
-- with that group's own placement: their entries are this move's now.
local function record_group(self, group, x, y)
    group = group:lower()
    local intent = self.intent
    intent.groups[group] = {x = x, y = y}
    intent.positions[group_anchor[group] or ''] = nil
    local carried = group_carries[group]
    if carried then
        intent.groups[carried] = nil
        intent.positions[group_anchor[carried]] = nil
    end
end

local function record_size(self, name, a, b)
    self.intent.sizes[name:lower()] = b and {w = a, h = b} or {rows = a}
end

-- A verb or read of one window.
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

for _, verb in ipairs({'hide', 'unhide', 'open', 'close'}) do
    Handle[verb] = window_call(verb, verb .. '(name)')
end

-- block(name[, category]) and unblock(name[, category]): a category names
-- one list of ability, by name or number, and reaches the engine only when
-- given, so a resident older than 0.10.0 still serves the plain call.
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

-- A read of no window.
local function read_call(verb)
    return function(self)
        check_self(self, verb)
        if not self.native then
            return nil, released
        end
        return read_result(verb, call(self, verb))
    end
end

for _, verb in ipairs({'list', 'opened', 'focused', 'remembered', 'groups', 'pending', 'rects', 'macros'}) do
    Handle[verb] = read_call(verb)
end

-- A verb of no window.
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

-- x, y are the frame's top-left: info(name).rect.x and .y.
function Handle:move(name, x, y)
    check_self(self, 'move')
    need(type(name) == 'string', 'move(name, x, y): name must be a string')
    need(type(x) == 'number' and type(y) == 'number', 'move(name, x, y): x and y must be numbers')
    if not self.native then
        return nil, released
    end
    local ok, why = call(self, 'move', name, x, y)
    if ok then
        self.intent.positions[name:lower()] = {x = x, y = y}
        note_ui(self)
    end
    return ok, why
end

-- The anchor's frame top-left to x, y, what it carries by as much. With the
-- anchor closed, the move waits for the game to open it.
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

-- handle:resize(name, rows) or handle:resize(name, w, h).
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
        record_size(self, name, a, b)
        note_ui(self)
    end
    return ok, why
end

-- handle:reset(name[, aspect]): the position and the size this handle
-- placed, or only the 'position' or the 'size'.
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

-- The group's anchor and everything this handle's move_group of it carried.
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

-- The window name and the id a reply names: a prompt table as options() or
-- pending() gave it, or the name and the id the caller passed. Misuse
-- raises at the addon's line.
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

-- handle:answer(prompt, value) or handle:answer(name, value[, id]): the
-- type of value each window takes is the engine's to say. The prompt's id
-- refuses the answer once the game has opened the window again, or for
-- prtyjoin and the post boxes once another invite or session has come.
function Handle:answer(prompt, value, id)
    check_self(self, 'answer')
    local name
    name, id = reply_target('answer(prompt, value) or answer(name, value[, id])', 'answer(name, value, id)', prompt, id)
    if not self.native then
        return nil, released
    end
    return call(self, 'answer', name, value, id)
end

-- handle:cancel(prompt) or handle:cancel(name[, id]).
function Handle:cancel(prompt, id)
    check_self(self, 'cancel')
    local name
    name, id = reply_target('cancel(prompt) or cancel(name[, id])', 'cancel(name, id)', prompt, id)
    if not self.native then
        return nil, released
    end
    return call(self, 'cancel', name, id)
end

-- What this handle asked for and the entries apply() could not apply, a
-- fresh table every call, the same on a released handle.
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

-- One section of a layout: absent is empty, anything but a table is misuse.
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

-- handle:apply(layout) -> true, or nil, every refusal as one line, and the
-- list of them, each {verb, name, reason}. Replays layout(): the group moves
-- first, in the order the groups carry each other, then the moves of single
-- windows, then the sizes. A refused entry does not stop the others, and
-- layout() keeps it. A layout saved at another UI size applies nothing.
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
        local key = tostring(name):lower()
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

-- Whether the engine has a window of that name. Until a handle has read the
-- names, any name passes.
local function known_window(self, name)
    if not window_names and self.native then
        local all = self.native:list()
        if type(all) == 'table' then
            window_names = {}
            for key in pairs(all) do
                window_names[key] = true
            end
        end
    end
    return not window_names or window_names[name:lower()] == true
end

-- The window name on(), off() take: nil, or a window's name for an event
-- that names one.
local function check_window(self, verb, event, name)
    if name == nil then
        return
    end
    if type(name) ~= 'string' then
        error(('%s(event, name, fn): name must be a window name'):format(verb), 3)
    end
    if unnamed[event] then
        error(('%s(event, name, fn): %s events name no window'):format(verb, event), 3)
    end
    if not known_window(self, name) then
        error(('%s(event, name, fn): no such window: %s'):format(verb, name), 3)
    end
end

-- The forms of on() and off(): (event, fn), (event, name, fn) and (event,
-- name, category, fn), off's each without the fn too. The name and the
-- category are the strings present, in that order.
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

-- The category on(), off() take: nil, or one list of ability by the name
-- or number its events carry; only ability's events carry one.
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

-- handle:on(event, fn), handle:on(event, name, fn) or handle:on(event,
-- name, category, fn) -> fn. fn(event) runs on the frame after it
-- happened; with a name, only for that window; with a category, only for
-- that list of ability.
function Handle:on(event, name, category, fn)
    check_self(self, 'on')
    name, category, fn = listener_args(name, category, fn)
    check_event(event)
    check_window(self, 'on', event, name)
    check_category('on', name, category)
    if type(fn) ~= 'function' then
        error('on(event[, name[, category]], fn) needs a function', 2)
    end
    local list = self.callbacks[event]
    list[#list + 1] = {fn = fn, name = name and name:lower(), category = category and category:lower()}
    return fn
end

-- handle:off(event [, name [, category]] [, fn]): the callbacks for the
-- event, those for one window, those for one list of ability, one
-- function, or one function for one window or list.
function Handle:off(event, name, category, fn)
    check_self(self, 'off')
    name, category, fn = listener_args(name, category, fn)
    check_event(event)
    check_window(self, 'off', event, name)
    check_category('off', name, category)
    if fn ~= nil and type(fn) ~= 'function' then
        error('off(event[, name[, category]], fn): fn must be a function', 2)
    end
    local list = self.callbacks[event]
    local window = name and name:lower()
    local wanted = category and category:lower()
    for i = #list, 1, -1 do
        local c = list[i]
        if (window == nil or c.name == window) and (wanted == nil or c.category == wanted)
                and (fn == nil or c.fn == fn) then
            table.remove(list, i)
        end
    end
    return true
end

-- status() as hideui.status() gives it, with this handle's own counts:
-- `dropped`, the events it lost, and detail.callback_errors.
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

-- Drops this handle's hides and blocks and resets the windows it moved or
-- resized last. layout() keeps answering with what the handle asked for.
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

-- hideui.new([name]) -> handle, or nil and the reason. The name, by
-- default _addon.name, labels this addon's windows in remembered() and
-- info().
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

-- status(): `dropped` counts the events every handle in the client lost;
-- detail.callback_errors the callbacks of this addon's handles that failed.
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

-- hideui.debug(true) lets this file print to chat -- why new() failed, the
-- errors and failing callbacks, lost events, a shutdown the engine could
-- not finish -- and leaves `detail` in what status(), info(), list() and
-- pending() return.
function hideui.debug(on)
    if type(on) ~= 'boolean' then
        error('hideui.debug(on): on must be true or false', 2)
    end
    debugging = on
end

-- A callback's category filter against the event's: the name the engine
-- gives the list, or its number.
local function category_matches(c, event)
    if c.category == nil or event.category_name == c.category then
        return true
    end
    local number = tonumber(c.category)
    return number ~= nil and event.category == number
end

-- A callback that fails is counted every time and posted to the handle as
-- an error event the first time; one failing on an error event is only
-- counted.
local function deliver(handle, event)
    local list = handle.callbacks[event.event]
    if not list or #list == 0 then
        return
    end
    local snapshot = {unpack(list)}
    for i = 1, #snapshot do
        local c = snapshot[i]
        if (c.name == nil or c.name == event.name) and category_matches(c, event) then
            local ok, message = pcall(c.fn, event)
            if not ok then
                handle.callback_errors = handle.callback_errors + 1
                if not c.failed then
                    c.failed = true
                    say(('%s: %s callback failed: %s'):format(handle.name, event.event, tostring(message)))
                    if event.event ~= 'error' then
                        deliver(handle, {event = 'error', verb = 'callback', name = event.name,
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
