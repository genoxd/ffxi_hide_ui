-- hideuidemo: the hideui library's calls as chat commands, to try them in
-- game. //hideuidemo help lists the commands.

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

local debugging = false

local function report(done, err, text)
    if done then
        say(207, text)
    else
        say(123, text .. ' failed: ' .. tostring(err))
    end
end

-- The library raises on misuse; a typed command reports it in chat instead.
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

local function event_target(event)
    local list = event.category_name or event.category
    return list and ('%s %s'):format(event.name, tostring(list)) or event.name
end

local function page_text(o)
    return ((o.raw or o.text or ''):gsub('\n', ' / '))
end

local function print_event(event)
    if event.event == 'page' then
        local o = call('options', event.name)
        say(207, ('page %s%s'):format(event.name, o and (': "' .. page_text(o) .. '"') or ''))
    elseif event.event == 'pending' then
        say(207, ('pending %s %s'):format(event.what, event.pending and 'appeared' or 'cleared'))
    elseif event.event == 'cursor' and event.name == 'query' then
        say(207, ('cursor query option %d'):format(event.row))
    elseif event.event == 'cursor' then
        say(207, ('cursor %s row %d'):format(event.name, event.row))
    elseif event.event == 'blocked' and type(event.by) == 'table' and #event.by > 0 then
        say(207, ('blocked %s (by %s%s)'):format(event_target(event), table.concat(event.by, ', '),
            event.mine and ', mine' or ''))
    else
        say(207, ('%s %s'):format(event.event, event_target(event)))
    end
end

local window_events = {'opened', 'closed', 'covered', 'uncovered', 'blocked', 'cursor', 'page'}

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

-- closeon shows that a window is drawn once before its opened event
-- arrives.
local closeon = {}

local function close_on_open(event)
    local want = closeon[event.name]
    if want == nil then
        return
    end
    if want ~= true then
        local p = call('info', event.name)
        if not p or p.category ~= want then
            return
        end
    end
    local ok, why = call('close', event.name)
    say(207, ok and ('closed %s on its opened event'):format(event.name)
        or ('closeon %s failed: %s'):format(event.name, tostring(why)))
end

-- trade and handover never open blocked, so unblock, not cancel, is their
-- way out.
local block_consequence = {
    delivery = 'the player cannot move until you cancel it and the server answers',
    post1    = 'the post-box session stays open until you cancel it and the server answers',
    post2    = 'the post-box session stays open until you cancel it and the server answers',
    passinpu = "the NPC's script waits until you answer or cancel",
    link5    = "the NPC's script waits until you answer or cancel",
    arealist = "the NPC's script waits until you answer or cancel",
    prtyjoin = 'the invite waits until you answer or cancel',
    moneyctr = 'each question waits with no window until you answer or cancel it',
    itemctrl = 'each question waits with no window until you answer or cancel it',
    sortyn   = 'each question waits with no window until you answer or cancel it',
    evitem01 = 'the key item list stays locked until you answer or cancel its question',
    maplist  = 'each question waits with no window until you answer or cancel it',
    bankmenu = 'no other Mog Safe picker opens until you answer or cancel the one waiting',
    ok       = 'the game waits on its notice until you answer it',
    mogdoor  = 'the door asks with no window until you answer or cancel it, or the player uses it again',
    msgline  = 'the message waits with no window until you answer or cancel it, or another is set up',
    trade    = 'the trade is canceled',
    handover = 'the NPC trade does not start',
}

-- Each window's last read prompt. answer and cancel pass it back, so its id
-- refuses a reply once a newer prompt has come.
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

-- query's and arealist's cursor and top index options(name)'s list, not
-- the rows on screen.
local function cursor_text(p)
    if p.name ~= 'query' and p.name ~= 'arealist' then
        return ('cursor %s'):format(tostring(p.cursor))
    end
    if not p.cursor then
        return p.name == 'query' and 'cursor on no option' or 'cursor on no row'
    end
    if not p.top then
        return ('cursor row %d'):format(p.cursor)    -- an older resident engine gives no top
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
    if resize.holds == 'none' then
        say(207, '  no size to set')
    else
        say(207, ('  a size holds until %s%s'):format(
            resize.holds == 'reopen' and 'it closes' or resize.holds == 'trigger' and 'its owner re-sizes it'
                or 'the next frame',
            resize.min_rows and (', resizes by rows %d..%d'):format(resize.min_rows, resize.max_rows) or ''))
    end
    if p.open then
        -- targetarrow has no rect, and the compass no elements.
        if p.rect then
            say(207, ('  rect %s, %s'):format(rect(p.rect), cursor_text(p)))
        end
        if p.category then
            say(207, p.category_name and ('  category %d (%s)'):format(p.category, p.category_name)
                or ('  category %d'):format(p.category))
        end
        local count = p.elements and #p.elements or 0
        if p.elements then
            say(207, ('  %d elements%s%s'):format(count, p.elements_truncated and ', truncated' or '',
                count > shown_elements and (' (first %d of %d)'):format(shown_elements, count) or ''))
        end
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
        if d.layout then
            say(207, ('debug: template %s, controller %s, state %s'):format(tostring(d.layout),
                tostring(d.controller), tostring(d.state)))
        end
    end
end

-- The plain text loses the runs the game draws in green.
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

local function number_text(o)
    return ('%s: %d (%d..%d)%s%s, prompt %d'):format(o.name, o.value, o.min, o.max,
        o.item and (' of item ' .. o.item) or '', o.blocked and ', blocked: no window' or '', o.id)
end

local function blocked_note(o)
    return o.blocked and ', blocked: no window' or ''
end

local function door_row_text(row)
    if not row.enabled then
        return 'turned off now'
    elseif row.value then
        return ('answer %d (%s%s)'):format(row.value, row.kind, row.zone and (' zone ' .. row.zone) or '')
    elseif row.kind == 'stay' then
        return 'cancel mogdoor'
    end
    return 'opens the list of areas; answer an area\'s value from either level'
end

-- The areas' values answer from the top level too, so it lists them.
local function show_door(o)
    say(207, ('mogdoor "%s": %s level%s, prompt %d'):format(o.title.raw or o.title.text, o.level, blocked_note(o),
        o.id))
    for i, row in ipairs(o.options) do
        say(207, ('  %d: "%s" -- %s'):format(i, row.raw or row.text, door_row_text(row)))
    end
    if o.level == 'top' then
        for i, area in ipairs(o.areas) do
            say(207, ('    area %d: "%s" -- %s'):format(i, area.raw or area.text, door_row_text(area)))
        end
    end
    say(207, '  answer mogdoor <value> or cancel mogdoor (stays)')
end

local function show_asked(o)
    if o.name == 'sortyn' or o.name == 'evitem01' then
        local first = o.default == nil and '' or (o.default and ', yes first' or ', no first')
        say(207, ('%s "%s"%s%s%s, prompt %d%s'):format(o.name, o.raw or o.text, undecoded_note(o.undecoded), first,
            blocked_note(o), o.id, green_note(o.segments)))
        if o.title then
            say(207, ('  title "%s"'):format(o.title.raw or o.title.text))
        end
        say(207, ('  answer %s yes|no or cancel %s'):format(o.name, o.name))
    elseif o.name == 'confyn' then
        say(207, ('confyn: restore the defaults? %s first, prompt %d'):format(o.default and 'yes' or 'no', o.id))
        say(207, '  answer confyn yes|no')
    elseif o.name == 'bankmenu' then
        local offered = {}
        for _, c in ipairs(o.containers) do
            offered[#offered + 1] = ('%d%s'):format(c.id, c.available and '' or ' (not available)')
        end
        say(207, ('bankmenu: containers %s%s, prompt %d'):format(table.concat(offered, ', '), blocked_note(o), o.id))
        say(207, '  answer bankmenu <container id> or cancel bankmenu')
    elseif o.name == 'maplist' then
        local level = o.top and ', the first level' or o.level == 'zone' and ', one zone\'s maps'
            or ', a region\'s list'
        say(207, ('maplist: %d rows%s%s, prompt %d'):format(#o.rows, level, blocked_note(o), o.id))
        for _, row in ipairs(o.rows) do
            local beside = row.label and (' [' .. (row.label_raw or row.label) .. ']')
                or row.count and (' [' .. row.count .. ']') or ''
            local what = row.kind == 'map' and ('zone %d map %d'):format(row.zone, row.map) or row.kind
            say(207, ('  %d: "%s"%s -- %s'):format(row.value, row.raw or row.text, beside,
                row.kind == 'list' and ('a list of its own, answer %d opens it'):format(row.value)
                    or ('answer %d (%s)'):format(row.value, what)))
        end
        say(207, o.top and '  answer maplist <row value> or cancel maplist'
            or '  answer maplist <row value> or cancel maplist (back up a level)')
        say(207, '  each level is a prompt of its own: options maplist reads the one an answer or cancel opens')
    elseif o.name == 'gmtell' then
        say(207, ('gmtell from %s: "%s"%s, %d options, prompt %d'):format(o.raw or o.gm, o.title.raw or o.title.text,
            undecoded_note(o.title.undecoded), #o.options, o.id))
        for i, option in ipairs(o.options) do
            say(207, ('  %d: "%s" -- answer %d%s'):format(i, option.raw or option.text, option.value,
                undecoded_note(option.undecoded)))
        end
        say(207, '  answer gmtell <option value> or cancel gmtell (declines)')
    elseif o.name == 'mp_dead' then
        say(207, ('mp_dead: %d:%02d on the countdown, prompt %d'):format(math.floor(o.seconds / 60), o.seconds % 60,
            o.id))
        for _, option in ipairs(o.options) do
            say(207, ('  %d: "%s"'):format(option.value, option.raw or option.text))
        end
        say(207, '  answer mp_dead 1|2 (the countdown running out chooses 2)')
    elseif o.name == 'ok' then
        say(207, ('ok "%s"%s%s, prompt %d'):format(o.raw or o.text, undecoded_note(o.undecoded), blocked_note(o),
            o.id))
        say(207, '  answer ok')
    elseif o.name == 'mogdoor' then
        show_door(o)
    end
end

local asked_windows = {sortyn = true, evitem01 = true, confyn = true, bankmenu = true, maplist = true,
                       gmtell = true, mp_dead = true, ok = true, mogdoor = true}

local function show_dialogue(o)
    say(207, ('%s: the page waits on %s%s%s, %s, %d queued, %s, prompt %d'):format(o.name, o.waits or 'nothing',
        o.seconds and (' (%.1f s left)'):format(o.seconds) or '', o.auto and ', the script confirms it' or '',
        o.more and 'more pages follow' or 'the last page', o.queued, o.cancellable and 'cancellable' or 'no cancel',
        o.id))
    for line in (o.raw or o.text):gmatch('[^\n]+') do
        say(207, ('  "%s"%s'):format(line, green_note(o.segments)))
    end
    say(207, ('  answer %s shows the next page%s'):format(o.name, o.cancellable and (', cancel ' .. o.name
        .. ' ends the event') or ''))
end

local function show_text(o)
    if o.name == 'stringdl' then
        if o.mode == 'name' then
            say(207, ('stringdl asks the recipient\'s name, %d to %d letters%s, prompt %d'):format(o.min_length,
                o.max_length, o.prompt and (' ("' .. (o.prompt_raw or o.prompt) .. '")') or '', o.id))
            say(207, '  answer stringdl <name> or cancel stringdl')
        else
            say(207, ('stringdl asks yes or no about a delivery slot%s, prompt %d'):format(o.prompt
                and (' ("' .. (o.prompt_raw or o.prompt) .. '")') or '', o.id))
            say(207, '  answer stringdl yes|no or cancel stringdl')
        end
    elseif o.name == 'comment' then
        say(207, ('comment: the %s message%s, prompt %d'):format(o.mode, o.category and (', category '
            .. o.category) or '', o.id))
        for i, line in ipairs(o.lines) do
            say(207, ('  line %d: "%s"'):format(i, line.raw or line.text))
        end
        say(207, o.mode == 'view' and '  another player\'s message: nothing to answer'
            or '  answer comment <line1>|<line2>|<line3> (it sends; comment has no cancel)')
    elseif o.name == 'msgline' then
        say(207, ('msgline mode %d: at most %d bytes%s%s, prompt %d'):format(o.mode, o.max_length, o.recipient
            and (' to ' .. (o.recipient_raw or o.recipient)) or '', blocked_note(o), o.id))
        say(207, '  answer msgline <text> or cancel msgline')
    elseif o.name == 'scname' then
        say(207, ('scname: a name of at most %d letters, now "%s", prompt %d'):format(o.max_length, o.raw or o.text,
            o.id))
        say(207, '  answer scname <name> or cancel scname')
    elseif o.name == 'link11' then
        say(207, ('link11: a name of at most %d letters, now "%s", colors %d %d %d, prompt %d'):format(o.max_length,
            o.raw or o.text, o.color[1], o.color[2], o.color[3], o.id))
        say(207, '  answer link11 <name> [<r> <g> <b>] or cancel link11')
    end
end

local text_windows = {stringdl = true, comment = true, msgline = true, scname = true, link11 = true}

local function offer_items(items)
    for _, item in ipairs(items) do
        say(207, ('    slot %d: %d of id %d%s%s'):format(item.slot, item.count, item.id,
            item.item and (', inventory ' .. item.item) or '', item.pending and ' (on its way to the server)' or ''))
    end
end

local function gray_items(o)
    if o.gray and #o.gray > 0 then
        say(207, '  grayed out by the game, cannot be offered: inventory ' .. table.concat(o.gray, ', '))
    end
end

local function show_offer(o)
    if o.name == 'trade' then
        say(207, ('trade with %s, prompt %d%s%s'):format(tostring(o.partner_raw or o.partner), o.id,
            o.ok_mine and ', your OK is in' or '', o.ok_theirs and ', their OK is in' or ''))
        say(207, ('  yours: %d gil, %d items'):format(o.mine.gil, #o.mine.items))
        offer_items(o.mine.items)
        say(207, ('  theirs: %d gil, %d items'):format(o.theirs.gil, #o.theirs.items))
        offer_items(o.theirs.items)
        if o.next_change and o.next_change > 0 then
            say(207, ('  the next change can go in %.1f s'):format(o.next_change))
        end
    else
        say(207, ('handover to %s, prompt %d: %d gil, %d items'):format(tostring(o.npc_raw or o.npc), o.id, o.gil,
            #o.items))
        offer_items(o.items)
        if o.next_ok and o.next_ok > 0 then
            say(207, ('  the OK can go in %.1f s'):format(o.next_ok))
        end
    end
    gray_items(o)
    say(207, ('  offer %s <item> <count> | offer %s gil <amount> | withdraw %s <item> | answer %s (the OK) | cancel'
        .. ' %s'):format(o.name, o.name, o.name, o.name, o.name))
end

local function show_options(name)
    local o, err = call('options', name)
    if not o then
        prompts[name] = nil
        say(123, 'options failed: ' .. tostring(err))
        return
    end
    prompts[name] = o
    if text_windows[o.name] then
        show_text(o)
    elseif o.name == 'trade' or o.name == 'handover' then
        show_offer(o)
    elseif asked_windows[o.name] then
        show_asked(o)
    elseif o.name == 'rem4line' or o.name == 'rem4li2' then
        show_dialogue(o)
    elseif o.options then
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
            or '  read-only: the player\'s keys drive this list; answer and cancel are for an NPC\'s prompt')
        for i, row in ipairs(o.rows) do
            local beside = row.label and (' [' .. (row.label_raw or row.label) .. ']') or row.count and (' [' .. row.count .. ']') or ''
            say(207, ('  %d: "%s"%s -- answer %d (%s)'):format(i, row.raw or row.text, beside, row.id, row.kind))
        end
    elseif o.zones then    -- an older resident engine's arealist
        local zones = {}
        for _, z in ipairs(o.zones) do zones[#zones + 1] = tostring(z.zone) end
        say(207, ('arealist: %d zones: %s'):format(#zones, table.concat(zones, ', ')))
    elseif o.max_length then
        say(207, ('passinpu: text of at most %d bytes'):format(o.max_length))
    elseif o.inviter then
        say(207, ('prtyjoin: %s invite from %s, prompt %d'):format(o.alliance and 'alliance' or 'party', o.raw or o.inviter,
            o.id))
    elseif o.text then
        say(207, ('%s "%s"%s: %s first%s%s, prompt %d%s'):format(o.name, o.raw or o.text, undecoded_note(o.undecoded),
            o.default and 'yes' or 'no', o.cancellable and ', cancellable' or ', no cancel',
            o.hold and ', confirmed by holding' or '', o.id, green_note(o.segments)))
        say(207, ('  answer %s yes|no%s'):format(o.name, o.cancellable and (' or cancel ' .. o.name) or ''))
    elseif o.seconds then
        say(207, ('dead: %d:%02d on the countdown, prompt %d'):format(math.floor(o.seconds / 60), o.seconds % 60, o.id))
        say(207, o.askable and '  answer dead asks its question through comyn; then options comyn and answer comyn'
            .. ' yes|no' or '  its question cannot be asked now: comyn has one pending')
    elseif o.max then
        say(207, number_text(o))
        say(207, ('  answer %s <%d..%d> or cancel %s'):format(o.name, o.min, o.max, o.name))
    end
end

local yes_or_no = {prtyjoin = true, comyn = true, merityn = true, resyn = true, resynus = true, resynde = true,
                   resynfr = true, shopbuy = true, shopsell = true, sortyn = true, evitem01 = true, confyn = true}

local no_value = {dead = true, ok = true, rem4line = true, rem4li2 = true, trade = true, handover = true}

local function answer_value(name, args)
    name = name:lower()
    local last = prompts[name]
    if name == 'passinpu' or name == 'msgline' or name == 'scname'
       or (name == 'stringdl' and not (last and last.mode == 'confirm')) then
        if args[2] == nil then return nil end
        return table.concat(args, ' ', 2)
    elseif name == 'stringdl' then
        local v = (args[2] or ''):lower()
        if v == 'yes' or v == 'true' then return true end
        if v == 'no' or v == 'false' then return false end
        return nil
    elseif name == 'comment' then
        if args[2] == nil then return nil end
        local lines = {}
        for line in (table.concat(args, ' ', 2) .. '|'):gmatch('([^|]*)|') do
            lines[#lines + 1] = line
        end
        return lines
    elseif name == 'link11' then
        if args[2] == nil then return nil end
        local value = {name = args[2]}
        if args[3] ~= nil then
            local r, g, b = tonumber(args[3]), tonumber(args[4]), tonumber(args[5])
            if not (r and g and b) then return nil end
            value.color = {r, g, b}
        end
        return value
    elseif yes_or_no[name] then
        local v = (args[2] or ''):lower()
        if v == 'yes' or v == 'accept' or v == 'true' then return true end
        if v == 'no' or v == 'decline' or v == 'false' then return false end
        return nil
    end
    return tonumber(args[2])
end

local function setting_text(v)
    if type(v) == 'table' then
        return ('%s %s %s'):format(tostring(v.r), tostring(v.g), tostring(v.b))
    elseif type(v) == 'boolean' then
        return v and 'on' or 'off'
    end
    return tostring(v)
end

local function setting_range(s)
    if s.choices then
        return table.concat(s.choices, '|')
    elseif s.kind == 'bool' then
        return 'on|off'
    elseif s.kind == 'color' then
        return ('r g b %s..%s'):format(tostring(s.min), tostring(s.max))
    end
    return ('%s..%s'):format(tostring(s.min), tostring(s.max))
end

local function show_settings(text)
    local list, err = call('settings')
    if not list then
        say(123, 'settings failed: ' .. tostring(err))
        return
    end
    local shown = 0
    for _, s in ipairs(list) do
        if not text or s.name:find(text, 1, true) or s.page == text then
            shown = shown + 1
            say(207, ('%s [%s %s] %s (%s, default %s)%s'):format(s.name, s.page, s.kind, setting_text(s.value),
                setting_range(s), setting_text(s.default), s.label and (': ' .. (s.label_raw or s.label)) or ''))
        end
    end
    if shown == 0 then
        say(207, 'no setting matches ' .. tostring(text))
    end
end

-- nil for words the setting's kind does not take. false and the reason
-- when settings() fails, so the failure is reported rather than a guess
-- sent. An unknown name is guessed at so set_setting gives its own refusal.
local function setting_value(name, args)
    local list, err = call('settings')
    if type(list) ~= 'table' then
        return false, tostring(err)
    end
    local kind
    for _, s in ipairs(list) do
        if s.name == name then
            kind = s.kind
        end
    end
    if kind == 'color' then
        local r, g, b = tonumber(args[2]), tonumber(args[3]), tonumber(args[4])
        return r and g and b and {r = r, g = g, b = b} or nil
    elseif kind == 'bool' then
        local v = (args[2] or ''):lower()
        if v == 'on' or v == 'true' then return true end
        if v == 'off' or v == 'false' then return false end
        return nil
    elseif kind == 'choice' then
        return args[2]
    end
    return tonumber(args[2]) or args[2]
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
    'hide|unhide <name>      invisible and open, takes no clicks, and a prompt no keys; a text window or passinpu'
        .. ' editing has its field ended / back; compass and targetarrow too: the compass, and the arrows over the'
        .. ' target (hide targetwi and they stay)',
    'block|unblock <name> [category]  closed if open, never opens again (implies hide) / back; ability alone takes'
        .. ' a category: job_abilities, pet_commands, weapon_skills, job_traits, ready, ... or a number',
    'move <name> <x> <y>     put a window\'s frame top-left at x,y (undocks it)',
    'group <group> <x> <y>   move chat_log, party_list or target_window with what is docked to it',
    'reset <name> [position|size]  back to the game\'s placement and size, only what this addon placed',
    'resetgroup <group>      the group\'s window and what this addon\'s move of it carried',
    'resetall                reset every window this addon moved or resized last',
    'open <name> [argument]  the game\'s own open of that window alone; ability and magic need a list:'
        .. ' open ability pet_commands; a window that takes an argument its value, or its fields as key=value:'
        .. ' open jbpcat thf, open iteminfo bag=inventory slot=5',
    'openset <set> [argument]  the game\'s own opener of a set: equipment, map, set_spells, attachments,'
        .. ' linkshell, instincts, macros, friend, mail, mount or status; ability or magic with a list;'
        .. ' synthesis or tabula with an inventory index; compare with an item; region with a kind;'
        .. ' text_colors with a category',
    'close <name>            the game\'s own close',
    'resize <name> <rows>    the family\'s template for that many rows',
    'resize <name> <w> <h>   set the frame\'s size',
    'info <name>             what the game holds for one window',
    'list [text]             every window, or with their state those whose name contains text',
    'opened                  the windows that are open now',
    'rects                   every open window\'s frame',
    'focused                 the window that has the keyboard',
    'remembered              remembered positions and sizes this session',
    'groups                  the dock groups, their windows and any move waiting',
    'options <name>          what query, link5, arealist, passinpu, prtyjoin, a yes/no window (comyn, merityn,'
        .. ' resyn, resynus, resynde, resynfr, shopbuy, shopsell, sortyn, evitem01, confyn), the KO window (dead),'
        .. ' a number box (moneyctr, itemctrl), bankmenu, maplist, gmtell, mp_dead, ok or mogdoor offers, the'
        .. ' page a dialogue box (rem4line, rem4li2) shows, what a text window (stringdl, comment, msgline,'
        .. ' scname, link11) asks for, or what a trade (trade, handover) holds and the inventory items the game grays'
        .. ' out for it',
    'answer <name> <value>   query <value>, link5 <slot>, arealist <zone id> (NPC prompt), passinpu <text>, prtyjoin'
        .. ' yes|no, a yes/no window yes|no, moneyctr or itemctrl <number>, bankmenu <container id>, maplist <row'
        .. ' value>, gmtell <option value>, mp_dead 1|2, mogdoor <exit value>; answer dead, no value, asks the KO'
        .. ' window\'s question through comyn; answer ok, no value, acknowledges the notice; answer rem4line or'
        .. ' rem4li2, no value, shows the next page; text as typed: stringdl <name> (yes|no on a slot), comment'
        .. ' <line1>|<line2>|<line3>, msgline <text>, scname <name>, link11 <name> [<r> <g> <b>] (colors 0..15);'
        .. ' answer trade or handover, no value, is the trade\'s OK',
    'cancel <name>           cancel a prompt (a number box, bankmenu, maplist, gmtell, mogdoor, the dialogue boxes,'
        .. ' stringdl, msgline, scname, link11, trade and handover too), or end a delivery or post-box session; the'
        .. ' box closes when the server answers; dead, confyn, mp_dead, ok and comment have no cancel',
    'offer <name> <item> <count>  trade or handover: count of the inventory item at index item into the first free'
        .. ' slot; an item the game grays out is refused',
    'offer <name> gil <amount>  trade or handover: the gil offered (0 takes it out)',
    'withdraw <name> <item>  trade or handover: that inventory item back out',
    'pending                 a party invite, post-box session, or a question a block kept from its window (a'
        .. ' number box, sortyn, evitem01, bankmenu, maplist, ok, mogdoor or msgline), waiting',
    'settings [text]         every setting of the game\'s Config menu, or those whose name contains text or whose'
        .. ' page is text: name, page, kind, value, range or choices, default and the game\'s label',
    'setting <name>          one setting\'s value',
    'set <name> <value>      set a setting as its page does: a number, on|off, a choice\'s name, a color as <r> <g>'
        .. ' <b>, a key\'s DirectInput code',
    'blockmacros|unblockmacros  stop the game\'s Ctrl+number and Alt+number macros / back',
    'macros                  whether the macro keys are blocked, and by whom',
    'layout                  save what this addon placed to data/settings.xml',
    'apply                   put the saved layout back',
    'events on [name [category]]  print events, cursor moves and dialogue pages too, as they happen, for every'
        .. ' window, one, or one list of ability',
    'events off              stop printing them',
    'closeon <name> [n] on|off  close the window on its opened event, only for list n of ability when given',
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
            or cmd == 'open' or cmd == 'openset' or cmd == 'close' then
        local lists = cmd == 'block' or cmd == 'unblock' or cmd == 'open' or cmd == 'openset'
        if not args[1] then
            say(123, 'usage: //hideuidemo ' .. cmd .. (cmd == 'openset' and ' <set>' or ' <name>')
                .. ((cmd == 'block' or cmd == 'unblock') and ' [category]' or lists and ' [argument]' or '')) return
        end
        local verb = cmd == 'openset' and 'open_set' or cmd
        local name = args[1]:lower()
        local category = lists and args[2] and args[2]:lower() or nil
        if category and (cmd == 'open' or cmd == 'openset') and category:find('=', 1, true) then
            local fields = {}
            for i = 2, #args do
                local key, value = args[i]:lower():match('^([%a_][%w_]*)=(.+)$')
                if not key then
                    say(123, 'usage: //hideuidemo ' .. cmd .. ' <name> key=value [key=value ...]') return
                end
                fields[key] = value
            end
            category = fields
        elseif category and (cmd == 'open' or cmd == 'openset') then
            -- the game's names have spaces, so the rest of the words are one name
            category = table.concat(args, ' ', 2):lower()
        end
        local done, err
        if category then
            done, err = call(verb, name, category)
        else
            done, err = call(verb, name)
        end
        local shown = type(category) == 'table' and table.concat(args, ' ', 2) or category
        report(done, err, verb .. ' ' .. name .. (shown and (' ' .. shown) or ''))
        if done and cmd == 'block' and not category and block_consequence[name] then
            say(207, ('  %s; way out: //hideuidemo %s %s'):format(block_consequence[name],
                (name == 'trade' or name == 'handover') and 'unblock' or name == 'ok' and 'answer' or 'cancel', name))
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
        if not args[1] then
            say(123, 'usage: //hideuidemo options <query|link5|arealist|passinpu|prtyjoin|a yes/no window|dead'
                .. '|moneyctr|itemctrl|bankmenu|maplist|gmtell|mp_dead|ok|mogdoor|rem4line|rem4li2|stringdl|comment'
                .. '|msgline|scname|link11|trade|handover>') return
        end
        show_options(args[1]:lower())

    elseif cmd == 'answer' then
        local name = args[1]
        local bare = name and no_value[name:lower()]
        local value
        if name and not bare then
            value = answer_value(name, args)
        end
        if not name or (value == nil and not bare) or (bare and args[2] ~= nil) then
            say(123, 'usage: //hideuidemo answer query|link5|arealist|moneyctr|itemctrl|bankmenu|maplist|gmtell'
                .. '|mp_dead|mogdoor <number> | passinpu|msgline|scname|stringdl <text> | prtyjoin|stringdl|<a'
                .. ' yes/no window> yes|no | comment <line1>|<line2>|<line3> | link11 <name> [<r> <g> <b>] | dead'
                .. ' | ok | rem4line | rem4li2 | trade | handover')
            return
        end
        local prompt = prompts[name:lower()]
        local id = prompt and prompt.id
        local shown = value
        if type(value) == 'table' then
            shown = value.name and (value.name .. (value.color and (' ' .. table.concat(value.color, ' ')) or ''))
                or table.concat(value, '|')
        end
        local done, err = call('answer', prompt or name:lower(), value)
        report(done, err, ('answer %s%s%s'):format(name:lower(), bare and '' or (' ' .. tostring(shown)),
            id and (' (prompt ' .. id .. ')') or ''))

    elseif cmd == 'offer' then
        local name, item, count = args[1], args[2], tonumber(args[3])
        if item then
            item = item:lower() == 'gil' and 'gil' or tonumber(item)
        end
        if not name or not item or not count then
            say(123, 'usage: //hideuidemo offer trade|handover <inventory index> <count> | offer trade|handover gil'
                .. ' <amount>') return
        end
        local done, err = call('offer', name:lower(), item, count)
        report(done, err, ('offer %s %s %d'):format(name:lower(), tostring(item), count))

    elseif cmd == 'withdraw' then
        local name, item = args[1], tonumber(args[2])
        if not name or not item then
            say(123, 'usage: //hideuidemo withdraw trade|handover <inventory index>') return
        end
        local done, err = call('withdraw', name:lower(), item)
        report(done, err, ('withdraw %s %d'):format(name:lower(), item))

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
        local waiting = 0
        for _, box in ipairs({'moneyctr', 'itemctrl'}) do
            if p[box] then
                prompts[box] = p[box]
                say(207, number_text(p[box]))
                waiting = waiting + 1
            end
        end
        for _, name in ipairs({'sortyn', 'evitem01', 'bankmenu', 'maplist', 'ok', 'mogdoor'}) do
            if p[name] then
                prompts[name] = p[name]
                show_asked(p[name])
                waiting = waiting + 1
            end
        end
        if p.msgline then
            prompts.msgline = p.msgline
            show_text(p.msgline)
            waiting = waiting + 1
        end
        if not p.invite and not p.post and waiting == 0 then say(207, 'nothing pending') end

    elseif cmd == 'settings' then
        show_settings(args[1] and args[1]:lower())

    elseif cmd == 'setting' then
        if not args[1] then say(123, 'usage: //hideuidemo setting <name>') return end
        local name = args[1]:lower()
        local value, err = call('setting', name)
        if value == nil then
            say(123, 'setting ' .. name .. ' failed: ' .. tostring(err))
        else
            say(207, ('%s = %s'):format(name, setting_text(value)))
        end

    elseif cmd == 'set' then
        local name = args[1] and args[1]:lower()
        local value, failed
        if name then
            value, failed = setting_value(name, args)
        end
        if failed then
            say(123, 'set failed: ' .. failed) return
        end
        if value == nil then
            say(123, 'usage: //hideuidemo set <name> <number | on|off | choice | r g b | key code>') return
        end
        local done, err = call('set_setting', name, value)
        report(done, err, ('set %s %s'):format(name, setting_text(value)))

    elseif cmd == 'blockmacros' or cmd == 'unblockmacros' then
        local verb = cmd == 'blockmacros' and 'block_macros' or 'unblock_macros'
        local done, err = call(verb)
        report(done, err, verb)

    elseif cmd == 'macros' then
        local m, err = call('macros')
        if not m then say(123, 'macros failed: ' .. tostring(err)) return end
        if m.blocked then
            say(207, ('macro keys blocked by %s%s'):format(table.concat(m.blocked_by, ', '), m.mine and ' (mine)' or ''))
        else
            say(207, 'macro keys not blocked')
        end

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

    elseif cmd == 'closeon' then
        local name = args[1] and args[1]:lower()
        local n = tonumber(args[2])
        local v = (n and args[3] or args[2] or ''):lower()
        if not name or (v ~= 'on' and v ~= 'off') then
            say(123, 'usage: //hideuidemo closeon <name> [n] on|off') return
        end
        local off, err = call('off', 'opened', name, close_on_open)
        if not off then say(123, 'closeon failed: ' .. tostring(err)) return end
        -- Keyed by the first name, which events carry.
        local p = call('info', name)
        local first = p and p.name or name
        closeon[first] = nil
        if v == 'on' then
            closeon[first] = n or true
            local done, err = call('on', 'opened', name, close_on_open)
            if not done then say(123, 'closeon failed: ' .. tostring(err)) return end
        end
        say(207, ('closeon %s%s %s'):format(name, n and (' list ' .. n) or '', v))
    elseif cmd == 'events' then
        local v = (args[1] or ''):lower()
        if v ~= 'on' and v ~= 'off' then
            say(123, 'usage: //hideuidemo events on [name [category]] | events off') return
        end
        stop_events()
        if v == 'off' then
            say(207, 'events off')
            return
        end
        local window = args[2] and args[2]:lower()
        local category = window and args[3] and args[3]:lower() or nil
        for _, event in ipairs(window_events) do
            local done, err
            if category then
                done, err = call('on', event, window, category, print_event)
            else
                done, err = call('on', event, window, print_event)
            end
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
        say(207, ('events on: %s%s%s'):format(window or 'every window, and pending',
            category and (' ' .. category) or '', dropped > 0 and (' (' .. dropped .. ' dropped so far)') or ''))
    end
end)
