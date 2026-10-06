-- hideuidemo - the hideui library's calls as commands.
--
--   //hideuidemo help                        every command, one line each
--   //hideuidemo hide|unhide <name>          invisible and open, takes no input (typed text still reaches passinpu) / back
--   //hideuidemo block|unblock <name>        closed if open, never opens again (implies hide) / back
--   //hideuidemo move <name> <x> <y>         put a window's frame top-left at x,y (undocks it)
--   //hideuidemo group <group> <x> <y>       move chat_log, party_list or target_window with what is docked to it;
--                                         what it carries follows; with the anchor closed, at its next open
--   //hideuidemo reset <name> [position|size]  back to the game's placement and size, or one of them;
--                                         only what this addon placed
--   //hideuidemo resetgroup <group>          the group's window and what this addon's move of it carried
--   //hideuidemo resetall                    reset every window this addon moved or resized last
--   //hideuidemo open|close <name>           the game's own open and close
--   //hideuidemo resize <name> <rows>        swap to the game's own layout for that many rows
--   //hideuidemo resize <name> <w> <h>       set the frame's size
--   //hideuidemo info <name>                 what the game holds for one window
--   //hideuidemo list [text]                 every window, or with their state those whose name contains text
--   //hideuidemo opened                      the windows that are open now
--   //hideuidemo rects                       every open window's frame
--   //hideuidemo focused                     the window that has the keyboard
--   //hideuidemo remembered                  remembered positions and sizes this session
--   //hideuidemo groups                      the dock groups, their windows and any move waiting
--   //hideuidemo options <name>              what query, link5, arealist, passinpu or prtyjoin offers
--   //hideuidemo answer <name> <value>       query <value>, link5 <slot>, arealist <zone id> (an NPC's
--                                         prompt; the search menu's list is read-only), passinpu
--                                         <text>, prtyjoin yes|no
--   //hideuidemo cancel <name>               cancel query, passinpu, link5, arealist (an NPC's prompt)
--                                         or prtyjoin; close the delivery box (its closing row) or the
--                                         post1/post2 box (its own cancel), which closes when the server
--                                         answers (answer and cancel reply to the prompt the last
--                                         options or pending read, and are refused once another has come)
--   //hideuidemo pending                     a party invite or post-box session waiting
--   //hideuidemo layout                      what this addon placed, saved to data/settings.xml
--   //hideuidemo apply                       put the saved layout back
--   //hideuidemo events on [name]            print opened/closed/covered/uncovered/blocked/cursor as they
--                                         happen, for one window, or for every window and pending ones too
--   //hideuidemo events off                  stop printing them
--   //hideuidemo debug on|off                the library's own diagnostics, and debug: lines in info and status
--   //hideuidemo status                      engine state and what it holds
--
-- Window names are the game's own, e.g. logwindo, logwin2, targetwi, equip.
-- Errors the game thread reports for this addon's calls, and lost events,
-- are always printed.
--
-- //lua unload hideuidemo undoes everything this addon did: hidden windows
-- come back, blocked ones can open, moved and resized ones go home.

_addon.name    = 'hideuidemo'
_addon.author  = 'Geno'
_addon.version = '0.3'
_addon.commands = {'hideuidemo'}

-- 123 is Windower's error color.
local function say(color, msg)
    windower.add_to_chat(color, '[hideuidemo] ' .. msg)
end

local loaded, hideui = pcall(require, 'libs.hideui')
if not loaded then
    local why = 'could not load hideui: ' .. tostring(hideui)
    say(123, why)
    windower.register_event('addon command', function() say(123, why) end)
    return
end

local config = require('config')

local ui, why_not = hideui.new()
if not ui then
    say(123, tostring(why_not))
end

-- Whether info and status print the library's detail table, which it only
-- fills while hideui.debug(true) is on.
local debugging = false

local function report(done, err, text)
    if done then
        say(207, text)
    else
        say(123, text .. ' failed: ' .. tostring(err))
    end
end

-- A verb or read, with misuse reported in chat rather than raised.
local function call(verb, ...)
    local results = {pcall(ui[verb], ui, ...)}
    if not results[1] then
        return nil, results[2]
    end
    return unpack(results, 2, table.maxn(results))
end

local function print_problem(event)
    if event.event == 'error' then
        say(123, ('error: %s %s: %s'):format(tostring(event.verb), event.name or '', tostring(event.reason)))
    else
        say(123, ('%d events lost; //hideuidemo opened and pending read the state again'):format(event.dropped))
    end
end

local function print_event(event)
    if event.event == 'pending' then
        say(207, ('pending %s %s'):format(event.what, event.pending and 'appeared' or 'cleared'))
    elseif event.event == 'cursor' and event.name == 'query' then
        say(207, ('cursor query option %d'):format(event.row))
    elseif event.event == 'cursor' then
        say(207, ('cursor %s row %d'):format(event.name, event.row))
    elseif event.event == 'blocked' and type(event.by) == 'table' and #event.by > 0 then
        say(207, ('blocked %s (by %s%s)'):format(event.name, table.concat(event.by, ', '), event.mine and ', mine' or ''))
    else
        say(207, ('%s %s'):format(event.event, event.name))
    end
end

local window_events = {'opened', 'closed', 'covered', 'uncovered', 'blocked', 'cursor'}

local function stop_events()
    for _, event in ipairs(window_events) do
        ui:off(event, print_event)
    end
    ui:off('pending', print_event)
end

if ui then
    ui:on('error', print_problem)
    ui:on('resync', print_problem)
end

-- What a block of these windows does to the game. cancel undoes each of them
-- except trade, which cancel does not take; unblock is its way out.
local block_consequence = {
    delivery = 'the player cannot move until you cancel it and the server answers',
    post1    = 'the post-box session stays open until you cancel it and the server answers',
    post2    = 'the post-box session stays open until you cancel it and the server answers',
    passinpu = "the NPC's script waits until you answer or cancel",
    link5    = "the NPC's script waits until you answer or cancel",
    arealist = "the NPC's script waits until you answer or cancel",
    prtyjoin = 'the invite waits until you answer or cancel',
    trade    = 'the trade is cancelled',
}

-- The prompt each window's last options or pending read gave, which answer
-- and cancel pass back: its name and id.
local prompts = {}

local function sorted_keys(t)
    local keys = {}
    for key in pairs(t) do
        keys[#keys + 1] = key
    end
    table.sort(keys)
    return keys
end

local function flags(p)
    local out = {}
    for _, field in ipairs({'open', 'hidden', 'blocked', 'moved', 'resized', 'docked', 'covered', 'focused'}) do
        if p[field] then out[#out + 1] = field end
    end
    if p.blockable == false then out[#out + 1] = 'hide-only' end
    return table.concat(out, ' ')
end

local function print_names(names, per_line)
    for i = 1, #names, per_line do
        say(207, '  ' .. table.concat(names, ', ', i, math.min(i + per_line - 1, #names)))
    end
end

local function rect(r)
    return ('%d,%d %dx%d'):format(r.x, r.y, r.w, r.h)
end

local function size_text(size)
    return size.rows and (size.rows .. ' rows') or (size.w .. 'x' .. size.h)
end

local function memory_text(m)
    local parts = {}
    if m.position then
        local p = m.position
        parts[#parts + 1] = ('at %d,%d%s owner %s%s'):format(p.x, p.y, p.group and (' (group ' .. p.group .. ')') or '',
            tostring(p.owner), p.mine and ' (mine)' or '')
    end
    if m.size then
        parts[#parts + 1] = ('size %s owner %s%s'):format(size_text(m.size), tostring(m.size.owner),
            m.size.mine and ' (mine)' or '')
    end
    return table.concat(parts, ', ')
end

local function holders_text(p)
    local parts = {}
    if p.hidden_by and #p.hidden_by > 0 then
        parts[#parts + 1] = 'hidden by ' .. table.concat(p.hidden_by, ', ')
    end
    if p.blocked_by and #p.blocked_by > 0 then
        parts[#parts + 1] = 'blocked by ' .. table.concat(p.blocked_by, ', ')
    end
    return table.concat(parts, '; ')
end

local shown_elements = 8

-- query's and arealist's cursor is the option or row under it, counted in
-- options(name), and top the first shown; every other window's is the row.
local function cursor_text(p)
    if p.name ~= 'query' and p.name ~= 'arealist' then
        return ('cursor %s'):format(tostring(p.cursor))
    end
    if not p.cursor then
        return p.name == 'query' and 'cursor on no option' or 'cursor on no row'
    end
    if not p.top then
        return ('cursor row %d'):format(p.cursor)    -- a resident engine older than 0.7.2 (0.7.3 for arealist)
    end
    local o = call('options', p.name)
    local list = o and (o.options or o.rows)
    local count = list and #list
    return ('cursor %d of %s (top %d)'):format(p.cursor, count and tostring(count) or '?', p.top)
end

local function show_info(name)
    local p, err = call('info', name)
    if not p then say(123, 'info failed: ' .. tostring(err)) return end
    local f = flags(p)
    say(207, ('%s%s'):format(p.name, f ~= '' and (': ' .. f) or ''))
    local holders = holders_text(p)
    if holders ~= '' then
        say(207, '  ' .. holders)
    end
    if p.memory then
        say(207, '  remembered ' .. memory_text(p.memory))
    end
    local resize = p.resize or {}
    say(207, ('  a size holds until %s%s'):format(
        resize.holds == 'reopen' and 'it closes' or resize.holds == 'trigger' and 'its owner re-sizes it'
            or 'the next frame',
        resize.min_rows and (', resizes by rows %d..%d'):format(resize.min_rows, resize.max_rows) or ''))
    if p.open then
        say(207, ('  rect %s, %s'):format(rect(p.rect), cursor_text(p)))
        local count = #p.elements
        say(207, ('  %d elements%s%s'):format(count, p.elements_truncated and ', truncated' or '',
            count > shown_elements and (' (first %d of %d)'):format(shown_elements, count) or ''))
        for i = 1, math.min(count, shown_elements) do
            local e = p.elements[i]
            local text = e.text and e.text ~= '' and (' "' .. (e.raw or e.text) .. '"') or ''
            local at = e.x and (' at %d,%d'):format(e.x, e.y) or ''
            local size = e.w and (' size %dx%d'):format(e.w, e.h) or ''
            say(207, ('    %d: %s%s%s%s'):format(i, e.type, at, size, text))
        end
    else
        say(207, '  not open')
    end
    local d = p.detail
    if debugging and type(d) == 'table' then
        if d.dock then
            say(207, 'debug: docks in group ' .. tostring(d.dock))
        end
        if d.live_layer then
            say(207, ('debug: live layer %d'):format(d.live_layer))
        end
    end
end

-- The runs the game draws in green, which the plain text does not show.
local function green_note(segments)
    local runs = {}
    for _, segment in ipairs(segments or {}) do
        if segment.color == 'green' and segment.text ~= '' then
            runs[#runs + 1] = '"' .. (segment.raw or segment.text) .. '"'
        end
    end
    if #runs == 0 then
        return ''
    end
    return ', ' .. table.concat(runs, ', ') .. ' in green'
end

local function undecoded_note(n)
    if not n or n == 0 then
        return ''
    end
    return (' (%d two-byte character%s shown as ?)'):format(n, n == 1 and '' or 's')
end

local function show_options(name)
    local o, err = call('options', name)
    if not o then
        prompts[name] = nil
        say(123, 'options failed: ' .. tostring(err))
        return
    end
    prompts[name] = o
    if o.options then
        say(207, ('query "%s"%s: %d options%s, prompt %d%s'):format((o.title.raw or o.title.text), undecoded_note(o.title.undecoded),
            #o.options, o.cancellable and ', cancellable' or ', no cancel', o.id, green_note(o.title.segments)))
        for i, option in ipairs(o.options) do
            say(207, ('  %d: "%s" -- answer %d%s%s'):format(i, option.raw or option.text, option.value,
                green_note(option.segments), undecoded_note(option.undecoded)))
        end
    elseif o.slots then
        say(207, ('link5: %d linkshells'):format(#o.slots))
        for _, s in ipairs(o.slots) do
            say(207, ('  slot %d: %s'):format(s.slot, s.raw or s.name))
        end
    elseif o.rows and #o.rows == 0 and o.pending and (o.mode == 1 or o.mode == 2) then
        say(207, ('arealist blocked, an NPC\'s prompt waiting (mode %d), prompt %d: answer arealist <zone id 0..511>'
            .. ' or cancel arealist'):format(o.mode, o.id))
    elseif o.rows then
        say(207, ('arealist mode %d level %d: %d rows, prompt %d'):format(o.mode, o.level, #o.rows, o.id))
        say(207, (o.mode == 1 or o.mode == 2)
            and '  an NPC\'s prompt: answer arealist <zone id> or cancel arealist'
            or '  read-only: the player\'s keys drive this list while it is not hidden; answer and cancel are for'
                .. ' an NPC\'s prompt')
        for i, row in ipairs(o.rows) do
            local beside = row.label and (' [' .. (row.label_raw or row.label) .. ']') or row.count and (' [' .. row.count .. ']') or ''
            say(207, ('  %d: "%s"%s -- answer %d (%s)'):format(i, row.raw or row.text, beside, row.id, row.kind))
        end
    elseif o.zones then    -- a resident engine older than 0.7.3
        local zones = {}
        for _, z in ipairs(o.zones) do zones[#zones + 1] = tostring(z.zone) end
        say(207, ('arealist: %d zones: %s'):format(#zones, table.concat(zones, ', ')))
    elseif o.max_length then
        say(207, ('passinpu: text of at most %d bytes'):format(o.max_length))
    elseif o.inviter then
        say(207, ('prtyjoin: %s invite from %s, prompt %d'):format(o.alliance and 'alliance' or 'party', o.raw or o.inviter,
            o.id))
    end
end

-- The answer's value as the window takes it: a number, the text, or yes/no.
local function answer_value(name, args)
    name = name:lower()
    if name == 'passinpu' then
        if args[2] == nil then return nil end
        return table.concat(args, ' ', 2)
    elseif name == 'prtyjoin' then
        local v = (args[2] or ''):lower()
        if v == 'yes' or v == 'accept' or v == 'true' then return true end
        if v == 'no' or v == 'decline' or v == 'false' then return false end
        return nil
    end
    return tonumber(args[2])
end

local function show_status_detail(s)
    local d = s.detail
    if not debugging or type(d) ~= 'table' then
        return
    end
    if d.error then
        say(123, 'debug: error ' .. tostring(d.error))
    end
    if d.daemon then
        say(207, ('debug: daemon abi %s build %s'):format(tostring(d.daemon.abi), tostring(d.daemon.build)))
    end
    if s.role == 'forwarder' and d.resident then
        say(207, ('debug: forwarding to the resident engine %s from %s'):format(tostring(d.resident.build),
            tostring(d.resident.path)))
    end
    if d.queued ~= nil then
        say(207, ('debug: queued %s, events %s, pinned %s%s'):format(tostring(d.queued), tostring(d.events),
            tostring(d.pinned), d.busy and ', busy' or ''))
    end
    if d.glyph_table then
        local g = d.glyph_table
        say(207, ('debug: glyph table: %d two-byte glyphs, %d by a code page 932 pair, %d gaiji, built in %.2f ms')
            :format(g.size or 0, g.cp932_defined or 0, g.gaiji or 0, g.build_ms or 0))
    end
    if (d.drain_errors or 0) > 0 then
        say(123, ('debug: %d drain error(s), last: %s'):format(d.drain_errors, tostring(d.drain_error)))
    end
    if (d.unmatched_open_keys or 0) > 0 then
        say(207, ('debug: %d open key(s) matched no window name'):format(d.unmatched_open_keys))
    end
    if (d.callback_errors or 0) > 0 then
        say(123, ('debug: %d callback error(s)'):format(d.callback_errors))
    end
end

local function show_status()
    local s, err = hideui.status()
    if not s then say(123, 'status failed: ' .. tostring(err)) return end
    if s.ok then
        say(207, ('%s, engine %s installed as %s'):format(hideui.version(), tostring(s.engine), tostring(s.role)))
    else
        say(123, ('%s, engine %s%s'):format(hideui.version(), tostring(s.state),
            why_not and (': ' .. tostring(why_not)) or ''))
    end
    if s.ui then
        say(207, ('UI %dx%d'):format(s.ui.w, s.ui.h))
    end
    say(207, ('handles %d, dropped %d'):format(s.handles or 0, s.dropped or 0))
    for _, field in ipairs({'hidden', 'blocked', 'moved', 'resized'}) do
        local names = s[field]
        if names and #names > 0 then
            say(207, field .. ': ' .. table.concat(names, ', '))
        end
    end
    show_status_detail(s)
end

-- data/settings.xml, read on first use: the layout command writes it, apply
-- reads it.
local settings
local function saved()
    settings = settings or config.load({layout = {}})
    return settings
end

local function show_layout(layout)
    if type(layout.ui) == 'table' then
        say(207, ('  at UI %dx%d'):format(layout.ui.w, layout.ui.h))
    end
    local any = false
    for _, section in ipairs({'groups', 'positions', 'sizes'}) do
        local t = type(layout[section]) == 'table' and layout[section] or {}
        for _, name in ipairs(sorted_keys(t)) do
            any = true
            local e = t[name]
            local what = section == 'sizes' and size_text(e) or ('%d,%d'):format(e.x, e.y)
            say(207, ('  %s %s %s'):format(section == 'groups' and 'group' or section == 'sizes' and 'size' or 'move',
                name, what))
        end
    end
    if not any then say(207, '  nothing placed') end
end

local help = {
    'help                    every command, one line each',
    'hide|unhide <name>      invisible and open, takes no input (typed text still reaches passinpu) / back',
    'block|unblock <name>    closed if open, never opens again (implies hide) / back',
    'move <name> <x> <y>     put a window\'s frame top-left at x,y (undocks it)',
    'group <group> <x> <y>   move chat_log, party_list or target_window with what is docked to it',
    'reset <name> [position|size]  back to the game\'s placement and size, only what this addon placed',
    'resetgroup <group>      the group\'s window and what this addon\'s move of it carried',
    'resetall                reset every window this addon moved or resized last',
    'open|close <name>       the game\'s own open and close',
    'resize <name> <rows>    the family\'s template for that many rows',
    'resize <name> <w> <h>   set the frame\'s size',
    'info <name>             what the game holds for one window',
    'list [text]             every window, or with their state those whose name contains text',
    'opened                  the windows that are open now',
    'rects                   every open window\'s frame',
    'focused                 the window that has the keyboard',
    'remembered              remembered positions and sizes this session',
    'groups                  the dock groups, their windows and any move waiting',
    'options <name>          what query, link5, arealist, passinpu or prtyjoin offers',
    'answer <name> <value>   query <value>, link5 <slot>, arealist <zone id> (NPC prompt), passinpu <text>, prtyjoin yes|no',
    'cancel <name>           cancel a prompt, or end a delivery or post-box session; the box closes when the server answers',
    'pending                 a party invite or post-box session waiting',
    'layout                  save what this addon placed to data/settings.xml',
    'apply                   put the saved layout back',
    'events on [name]        print events, cursor moves too, as they happen, for every window or one',
    'events off              stop printing them',
    'debug on|off            the library\'s diagnostics, and debug: lines in info and status',
    'status                  engine state and what it holds',
}

local known = {}
for _, line in ipairs(help) do
    for name in line:match('^(%S+)'):gmatch('[^|]+') do
        known[name] = true
    end
end

local function show_help()
    for _, line in ipairs(help) do
        say(207, '//hideuidemo ' .. line)
    end
end

windower.register_event('addon command', function(cmd, ...)
    cmd = (cmd or 'help'):lower()
    local args = {...}
    if not known[cmd] then
        say(123, 'no such command: ' .. cmd)
        show_help()
        return
    end
    if cmd == 'help' then
        show_help()
        return
    end
    if cmd == 'status' then
        show_status()
        return
    end
    if cmd == 'debug' then
        local v = (args[1] or ''):lower()
        if v ~= 'on' and v ~= 'off' then say(123, 'usage: //hideuidemo debug on|off') return end
        debugging = v == 'on'
        hideui.debug(debugging)
        say(207, 'debug ' .. v)
        return
    end
    if not ui then
        say(123, 'hideui did not start: ' .. tostring(why_not))
        return
    end

    if cmd == 'reset' then
        local name, aspect = args[1], args[2] and args[2]:lower()
        if not name or (aspect and aspect ~= 'position' and aspect ~= 'size') then
            say(123, 'usage: //hideuidemo reset <name> [position|size]') return
        end
        local done, err = call('reset', name:lower(), aspect)
        report(done, err, 'reset ' .. name:lower() .. (aspect and (' ' .. aspect) or ''))

    elseif cmd == 'resetgroup' then
        if not args[1] then say(123, 'usage: //hideuidemo resetgroup <chat_log|party_list|target_window>') return end
        local done, err = call('reset_group', args[1]:lower())
        report(done, err, 'reset_group ' .. args[1]:lower())

    elseif cmd == 'hide' or cmd == 'unhide' or cmd == 'block' or cmd == 'unblock'
            or cmd == 'open' or cmd == 'close' then
        if not args[1] then say(123, 'usage: //hideuidemo ' .. cmd .. ' <name>') return end
        local name = args[1]:lower()
        local done, err = call(cmd, name)
        report(done, err, cmd .. ' ' .. name)
        if done and cmd == 'block' and block_consequence[name] then
            say(207, ('  %s; way out: //hideuidemo %s %s'):format(block_consequence[name],
                name == 'trade' and 'unblock' or 'cancel', name))
        end

    elseif cmd == 'cancel' then
        if not args[1] then say(123, 'usage: //hideuidemo cancel <name>') return end
        local name = args[1]:lower()
        local prompt = prompts[name]
        local done, err = call('cancel', prompt or name)
        report(done, err, 'cancel ' .. name .. (prompt and prompt.id and (' (prompt ' .. prompt.id .. ')') or ''))
        if done and (name == 'delivery' or name == 'post1' or name == 'post2') then
            say(207, '  the box closes when the server answers')
        end

    elseif cmd == 'move' or cmd == 'group' then
        local name, x, y = args[1], tonumber(args[2]), tonumber(args[3])
        if not name or not x or not y then say(123, 'usage: //hideuidemo ' .. cmd .. (cmd == 'group' and ' <group>' or ' <name>') .. ' <x> <y>') return end
        local verb = cmd == 'move' and 'move' or 'move_group'
        local done, err = call(verb, name:lower(), x, y)
        report(done, err, ('%s %s to %d,%d'):format(verb, name:lower(), x, y))

    elseif cmd == 'resetall' then
        local done, err = call('reset_all')
        report(done, err, 'reset_all')

    elseif cmd == 'resize' then
        local name, a, b = args[1], tonumber(args[2]), tonumber(args[3])
        if not name or not a or (args[3] and not b) then
            say(123, 'usage: //hideuidemo resize <name> <rows> | resize <name> <w> <h>') return
        end
        local done, err = call('resize', name:lower(), a, b)
        report(done, err, ('resize %s to %s'):format(name:lower(), b and (a .. 'x' .. b) or (a .. ' rows')))

    elseif cmd == 'info' then
        if not args[1] then say(123, 'usage: //hideuidemo info <name>') return end
        show_info(args[1]:lower())

    elseif cmd == 'list' then
        local all, err = call('list')
        if not all then say(123, 'list failed: ' .. tostring(err)) return end
        local text = args[1] and args[1]:lower()
        local names, total = {}, 0
        for _, name in ipairs(sorted_keys(all)) do
            total = total + 1
            if not text then
                names[#names + 1] = name
            elseif name:find(text, 1, true) then
                local f = flags(all[name])
                names[#names + 1] = f ~= '' and (name .. ' [' .. f .. ']') or name
            end
        end
        if text then
            say(207, ('%d of %d windows:'):format(#names, total))
            print_names(names, 4)
        else
            say(207, ('%d windows:'):format(total))
            print_names(names, 8)
        end

    elseif cmd == 'opened' then
        local names, err = call('opened')
        if not names then say(123, 'opened failed: ' .. tostring(err)) return end
        say(207, ('%d open:'):format(#names))
        print_names(names, 8)

    elseif cmd == 'rects' then
        local all, err = call('rects')
        if not all then say(123, 'rects failed: ' .. tostring(err)) return end
        local names = sorted_keys(all)
        say(207, ('%d open:'):format(#names))
        for _, name in ipairs(names) do
            say(207, ('  %s %s'):format(name, rect(all[name])))
        end

    elseif cmd == 'focused' then
        local name, err = call('focused')
        if name == nil then say(123, 'focused failed: ' .. tostring(err)) return end
        say(207, 'focused: ' .. (name or 'none'))

    elseif cmd == 'remembered' then
        local all, err = call('remembered')
        if not all then say(123, 'remembered failed: ' .. tostring(err)) return end
        local names = sorted_keys(all)
        for _, name in ipairs(names) do
            say(207, name .. ' ' .. memory_text(all[name]))
        end
        if #names == 0 then say(207, 'no remembered positions or sizes') end

    elseif cmd == 'groups' then
        local all, err = call('groups')
        if not all then say(123, 'groups failed: ' .. tostring(err)) return end
        for _, key in ipairs({'chat_log', 'party_list', 'target_window'}) do
            local g = all[key]
            local at = g.origin and (' at %d,%d'):format(g.origin.x, g.origin.y) or ''
            say(207, ('%s: %s (%s%s), %d windows move with it'):format(
                key, g.anchor, g.anchor_open and 'open' or 'closed', at, #g.members))
            if g.waiting then
                say(207, ('  a move to %d,%d waits for it to open, owner %s%s'):format(g.waiting.x, g.waiting.y,
                    tostring(g.waiting.owner), g.waiting.mine and ' (mine)' or ''))
            end
        end

    elseif cmd == 'options' then
        if not args[1] then say(123, 'usage: //hideuidemo options <query|link5|arealist|passinpu|prtyjoin>') return end
        show_options(args[1]:lower())

    elseif cmd == 'answer' then
        local name = args[1]
        local value = name and answer_value(name, args)
        if not name or value == nil then
            say(123, 'usage: //hideuidemo answer query|link5|arealist <number> | passinpu <text> | prtyjoin yes|no')
            return
        end
        local prompt = prompts[name:lower()]
        local id = prompt and prompt.id
        local done, err = call('answer', prompt or name:lower(), value)
        report(done, err, ('answer %s %s%s'):format(name:lower(), tostring(value), id and (' (prompt ' .. id .. ')') or ''))

    elseif cmd == 'pending' then
        local p, err = call('pending')
        if not p then say(123, 'pending failed: ' .. tostring(err)) return end
        if p.invite then
            prompts.prtyjoin = p.invite
            say(207, ('%s invite from %s'):format(p.invite.alliance and 'alliance' or 'party', (p.invite.raw or p.invite.inviter)))
        end
        if p.post then
            prompts[p.post.box] = p.post
            say(207, 'post-box session open: ' .. tostring(p.post.box))
        end
        if not p.invite and not p.post then say(207, 'nothing pending') end

    elseif cmd == 'layout' then
        local layout, err = call('layout')
        if not layout then say(123, 'layout failed: ' .. tostring(err)) return end
        local s = saved()
        s.layout = layout
        local wrote, why = pcall(config.save, s, 'all')
        if wrote then
            say(207, 'layout saved to data/settings.xml:')
        else
            say(123, 'layout not saved: ' .. tostring(why))
        end
        show_layout(layout)

    elseif cmd == 'apply' then
        local layout = saved().layout
        if type(layout) ~= 'table' then layout = {} end
        local done, err, refused = call('apply', layout)
        if done then
            say(207, 'apply')
        elseif type(refused) ~= 'table' then
            say(123, 'apply failed: ' .. tostring(err))
        else
            say(123, ('apply: %d entr%s not applied, kept in the layout:'):format(#refused,
                #refused == 1 and 'y' or 'ies'))
            for _, r in ipairs(refused) do
                say(123, ('  %s %s: %s'):format(r.verb, r.name, r.reason))
            end
        end

    elseif cmd == 'events' then
        local v = (args[1] or ''):lower()
        if v ~= 'on' and v ~= 'off' then say(123, 'usage: //hideuidemo events on [name] | events off') return end
        stop_events()
        if v == 'off' then
            say(207, 'events off')
            return
        end
        local window = args[2] and args[2]:lower()
        for _, event in ipairs(window_events) do
            local done, err = call('on', event, window, print_event)
            if not done then
                stop_events()
                say(123, 'events on failed: ' .. tostring(err))
                return
            end
        end
        if not window then
            ui:on('pending', print_event)
        end
        local s = call('status')
        local dropped = s and s.dropped or 0
        say(207, ('events on: %s%s'):format(window or 'every window, and pending',
            dropped > 0 and (' (' .. dropped .. ' dropped so far)') or ''))
    end
end)
