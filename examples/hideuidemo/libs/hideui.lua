--[[
    hideui - hide, block, move, resize, open, close and answer the windows of FFXI's own interface.

        local hideui = require('libs.hideui')
        local ui = assert(hideui.new())      -- named after _addon.name

        ui:hide('logwindo')                  -- invisible, still open, takes no clicks
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
        opened     the game opened the window; none for an open of a window
                   already open, but for ability on another list
        closed     the window left the screen and is gone; opened and closed
                   alternate for each window
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
        page       an NPC dialogue box, rem4line or rem4li2, shows a new
                   page: options(name) reads it; when the dialogue ends the
                   box's closed event follows
        error      the game thread refused a call this handle queued:
                   .verb, .name and .reason. A group move waiting for its
                   anchor that another handle's move_group replaced is one,
                   .verb 'move_group'. A callback of this handle that fails
                   is posted once as an error too, .verb 'callback'.
        pending    a party invite, a post-box session or a question a
                   block kept from its window appeared or cleared: .what
                   'invite', 'post', or the window's name ('moneyctr',
                   'itemctrl', 'sortyn', 'evitem01', 'bankmenu', 'maplist',
                   'ok', 'mogdoor' or 'msgline'), .pending true or false
        resync     events were lost, .dropped of them, and the rest of this
                   frame's came before it: read the state again
    All but pending and resync name their window in .name; on(event, name,
    fn) takes a window name for those alone. On ability, opened, closed,
    covered, uncovered and blocked carry .category, the list the window
    shows or was asked for, and .category_name, its name (1 job_abilities,
    2 pet_commands, 3 weapon_skills, 4 job_traits, 6 blood_pact_rage, 8
    phantom_roll, 9 quick_draw, 10 blood_pact_ward, 11 sambas, 12 waltzes,
    13 steps, 14 flourishes_i, 15 stratagems, 16 jigs, 17 flourishes_ii, 18
    ready, 19 flourishes_iii, 20 monstrosity, 21 rune_enchantment, 22 ward,
    23 effusion); on(event, 'ability', category, fn) takes one of those
    names or a number, and off(event, 'ability', category[, fn]) the same.
    Events start at new(): sync with ui:opened(), ui:info() and ui:pending()
    after it.

    open(name[, argument]) opens exactly the window named, through the
    game's own open, and refuses, saying what to use instead, a window that
    cannot work alone: one whose open alone can crash the client, that opens
    only for the server or an NPC, or that sends a request made of what only
    the game's own route sets (inventor, bank and some fifty more, and any
    window whose controller does not exist yet); a set's main window
    (equip, map0, mapframe, automato, mp_mncst, tskill1, mcr1edit, mcr2edit,
    cnqframe), pointing at its set; and one that opens empty or with what
    the game left in it (evitem, emote). A window that acts on another's
    selection or state opens only while one of the windows it opens from is
    open, and is refused with "<name> needs <window> open" otherwise (gift
    with trade, the auction lists with auc1, quest01 with quest00, itemxinf
    with iteminfo, the comment sub-lists with comgenre or scresult, and the
    rest the README lists). mcr1edlo, mcr2edlo and jobcsel are other names
    of mcr1edit, mcr2edit and jobcselu: either name works, and events and
    lists report the first. The open hands the game's open routine what the
    game's own openers hand it: gaugewin, targetwi, playermo and mp_pmode
    open underneath a window they may not show over, as the game opens them,
    every other window at once.

    The argument is a string, a number (also as its digits in a string), or
    a table of named fields, each a string or a number; it
    reaches the engine as one string, so a string argument may hold neither
    '=' nor ';'. open('ability', list) opens one list of ability alone, by
    the names above or the number; open('magic', list) one list of magic
    alone (1 white_magic, 2 black_magic, 3 summoning, 4 ninjutsu, 5 songs, 6
    blue_magic, 7 geomancy, 8 trust), without the detail pane subwindo.
    Neither opens without a list. The windows below run the game's own route
    to them, the one its menu row, command or list runs, with what that
    route takes; a window that route opens along with the one named is held
    off. A default is the value of a plain open(name); "required" means a
    plain open is refused, whichever version of the library asks. Names the
    game gives are its own text, matched in any case.
        storage             none: the Mog House's storage list; only in the
                            Mog House
        storage2            none: the Nomad Moogle's storage list; only where
                            a Nomad Moogle serves
        mogpost             'mog_house' (default; only in the Mog House) or
                            'nomad' (in the Mog House or where a Nomad Moogle
                            serves)
        friend, msglist     none: the friend list, PlayOnline's mail (the
                            game can refuse it while busy); titlehan held off
        blklist             none: the blacklist
        mount               none: the mount list as the ability list opens
                            it; subwindo held off
        jobcselu            'main' or 'support', required: the job list for
                            that job; only in the Mog House or where a Nomad
                            Moogle serves. A pick in it changes the job
        conf11l             1 (default) or 2: the configuration menu's
                            message row of that number
        jbpcat              a job, 'war' to 'run' or 1 to 22, required:
                            refused for a job under level 99, as the game's
                            list does
        fp_cat2             1 to 7, required: one category of fp_cat
        quest00             a mode: 1 (default), 2 or 'Unity Information', 3
                            or 'Ranking Information'
        quest01             1 to 7 (default 1): the list of quest00's row of
                            that number, 1 to 5 from its mode 1, 6 and 7 from
                            its mode 3; needs quest00 open
        miss00              none (default): your nation's log; or a log:
                            "San d'Oria", 'Bastok', 'Windurst', 'Rise of the
                            Zilart', 'Chains of Promathia', 'Assault',
                            'Treasures of Aht Urhgan', 'Campaign', 'Wings of
                            the Goddess', 'Seekers of Adoulin', "Rhapsodies
                            of Vana'diel", 'The Voracious Resurgence', 'A
                            Crystalline Prophecy', "A Moogle Kupo d'Etat", 'A
                            Shantotto Ascension' (or 1 to 15); refused when
                            its expansion is not on
        searchjo, searchna, searchra, searchle, scchar
                            'new' (default: the search reset first) or
                            'refine' (refused before a search has run)
        comgenre            'comment', 'search' or 'refine', required
        sccompar            {purpose = 'comment' or 'refine', category = 1
                            to 3}, required; needs comgenre or scresult open
        sccomite, sccomls   'comment' or 'refine', required; needs comgenre
                            or scresult open
        scsibori            none: refused before a search has run; needs
                            scresult open
        iteminfo            {bag = name or Windower's bag id 0 to 17,
                            slot = 1 to 80} or {item = id 1 to 65534},
                            required. An empty slot, or one past the bag's
                            size now, is refused, and so is an item of types
                            20 to 23 by id alone, which the panel reads the
                            bag's copy of
        itemxinf            as iteminfo, the compare option's panel;
                            itemyinf held off; needs iteminfo open
        casttime            seconds, or {seconds =, style = 'bar' (default)
                            or 'countdown'}, required: 1/60 to 35791394.11,
                            in sixtieths of a second as the game counts
        eventtim            seconds as for casttime, required
        persona             none: the equipment screen's layout, alone
        macro               {palette = 'ctrl' or 'alt', row = 1 to 10},
                            required: on the other palette the game's palette
                            switch first; mcrledit held off
        scanlist            none: while the map tracks someone, the Wide
                            Scan row's question on comyn first, a yes
                            opening the list; needs map0 open
        coloresu            2 to 9, required; needs cnqframe open on its
                            Colonization map

    open_set(set[, argument]) runs the game's own opener of a set of
    windows, as the player's menu row, command or item use runs it:
    'equipment' (equip, inventor, persona, money and iteminfo, as the game's
    equipment menu opens them; refused in Monstrosity); 'ability' or 'magic'
    with a list, opening the list with its sort window and detail pane;
    'map' (mapframe and map0); 'set_spells' (the blue magic set: persona,
    bluehelp, bluinven, bluepoin, bluequip); 'attachments' (the automaton's:
    bluehelp, bluinven, statpup, automato); 'linkshell' (link5 and link6);
    'instincts' (mp_stat, level, mp_cstm, mp_mncst; only in Monstrosity);
    'synthesis' with the inventory index of a crystal (the main menu's
    Items row first, then inventor, iteminfo, tskill1); 'tabula' with the
    inventory index of a Maze Tabula (prvdungp, bluehelp, bluinven);
    'macros' (the macro editor on the current macro set: mcrbedit,
    mcr1edit, mcr2edit); 'friend' (friend and titlehan); 'mail' (msglist
    and titlehan); 'mount' (mount, abisortw and subwindo); 'compare' with an
    item as iteminfo takes one (itemxinf and itemyinf; needs iteminfo open);
    'status' (the Status row: persona with level and statcom2, or mp_stat's
    in Monstrosity); 'region' with 'Conquest', 'Besieged', 'Campaign' or
    'Colonization', in any case (cnqframe and that kind's points window,
    for the area you are in); 'text_colors' with a category 1 to 4
    (textcol2 and textcol1). A block on any window of the set, or on the list, refuses it
    up front; a set the game does not open, or a list it closes again at
    once (a pet's list with no pet out), comes back as an error event, "the
    game refused it". Each window posts its own events, and hide stays one
    window's. close of a set's window does what the game's Escape on it
    does: of mcr1edit or mcr2edit it closes the macro editor; of persona or
    mp_stat with statcom2 open, the Status screen; of textcol1, textcol2
    with it; of mp_mncst, mp_stat and mp_cstm with it; of cnqframe, its
    points windows and Colonization panes, the map fading out and closing
    a moment later. mp_stat alone is refused: use open_set('status'). A
    close, or a block's close, of a window another opens from (quest00
    under quest01, comgenre under sccompar, iteminfo under itemxinf, and
    the rest the README lists) closes each such child no other parent of it
    keeps open: first, through its own close with its own closed event,
    unless the game's own close of the parent closes it.

    A hidden window is not painted, but the game runs it exactly as if
    shown: it opens, closes and is cleaned up on the game's own schedule,
    keeps its timers and does its own work. A hidden window the game closes
    really goes away: it leaves opened(), and its closed event comes when it
    would for a shown window. unhide() shows a window as it stands then.
    Keys that reach a hidden window that is not asking are treated as for a
    shown one, down to the game ignoring them during the brief delay after
    it opens. A hidden window costs as much each frame as a shown one.

    No click lands on a hidden window. A hidden window keeps its place among
    the game's windows and gets or loses the focus as it would shown. A
    hidden prompt -- a window that asks the player something: query,
    passinpu, prtyjoin, delivery, post1, post2, link5, arealist, the yes/no
    windows comyn, merityn, resyn, resynus, resynde, resynfr, shopbuy and
    shopsell, the KO window dead, the number boxes moneyctr and itemctrl,
    sortyn, evitem01, confyn, bankmenu, maplist, gmtell, mp_dead, ok, the
    Mog House's exit mogdoor, the NPC dialogue boxes rem4line and rem4li2,
    the text-entry windows stringdl, comment, msgline, scname and link11,
    and the trade windows trade and handover -- takes none of the player's
    keys or gamepad buttons either while it asks, and the pointer stays in
    world mode over it; a click anywhere does not advance a hidden dialogue
    box. The addon owns whatever it draws in its place: it reads keys
    through Windower and replies with answer and cancel. Escape does not
    close a hidden prompt that asks, so an addon that hides one must answer
    or cancel it. A prompt asks unless options() says nothing is pending in
    it or that it is not open; arealist asks only while an NPC waits on it
    (mode 1 or 2), and delivery, post1 and post2 whenever they are up. A
    hidden prompt that asks nothing takes the player's keys as it would
    shown, and the game can leave one up, or focus it again, with nothing
    to answer: once the player sends a delivery it focuses stringdl again,
    and the player's Escape leaves it. An addon's own interface should
    expect those keys. Any other hidden window still takes the keys the game
    sends it: the party list, which <stpt> and <stal> drive through its
    keys, keeps working hidden. Typed text goes to the game's text entry
    rather than to a window, so a text-entry window or passinpu that edits
    when it is hidden, opens hidden already editing, or has its field
    started by a key while hidden has its field ended, as the window's own
    end does, dropping what was typed there; passinpu's, comment's and
    msgline's fields start again when it is shown, as their opens start
    them, comment's on the line it was on.

    info(name).cursor is the open window's cursor row; info('query').cursor
    is the option under the cursor, as the cursor event's .row, and .top the
    first option shown (both 1-based in options('query').options), so a list
    drawn three rows at a time scrolls as the game's does. info('arealist')
    is the same, 1-based in options('arealist').rows.

    block(name) refuses the window's opens and hides it, and closes it
    through the game's own close when it is open (an open prompt is left to
    its reply). query, gmtell, comyn, the resyn windows, dead, mp_dead,
    rem4line, rem4li2 and stringdl are never blocked or closed, whichever
    copy of the library asks: block() and close() refuse them and say why;
    hide them instead. close('ok') is refused too: the game waits until ok
    is acknowledged, so answer it instead. Any other prompt cancel() ends is
    refused only while it asks, by an error event once the game thread
    looks; one asking nothing closes as any window, link5 showing the
    linkshell list among them. gift is never blocked, and not
    closed while trade is open: the trade's keys read gift's window. block('ability', category)
    refuses one list of the ability window alone, the others opening as
    before, and closes the window when it shows that list;
    unblock('ability', category) drops that hold, and unblock('ability') the
    whole-window one alone. info('ability').blocked_categories lists the
    lists any handle blocks, .mine.blocked_categories this handle's. move
    and move_group take the frame's top-left, info(name).rect's x and y, for
    every window. reset(name[, 'position' | 'size']) and reset_group(group)
    reset only what this handle placed, and refuse, naming who placed it,
    when another handle did.

    targetarrow names the arrows the target window draws over the target,
    the second of them while the player picks a command's target. Like the
    compass it is no window, and takes hide, block, open and close as holds:
    hide, block and close keep the arrows off the screen, unhide, unblock
    and open bring them back, and no event follows a hold. Its opened and
    closed events say when the game starts and stops drawing an arrow.
    Hiding targetwi leaves its arrows on the screen; blocked, targetwi never
    opens and no arrow is drawn. move and resize are accepted and change
    nothing, the arrows following the target, and info('targetarrow') and
    rects() give no box for them.

    block_macros() stops the game's macro keys, Ctrl+number and
    Alt+number, and closes a macro bar that is up; unblock_macros() drops
    this handle's hold, and the keys stay blocked while another handle
    holds one. macros() is {blocked, blocked_by, mine}: whether they are
    blocked, the handles holding a block, and whether this one is one;
    hideui.status().macros_blocked says whether any does. The macro keys
    are no window: no name reaches them and no event names them.

    answer and cancel take the table options() or pending() returned (its
    name and id) in place of the window's name. Text the game shows comes
    back as UTF-8 in .text (or .name, .inviter, .gm), its Shift-JIS bytes in
    .raw beside it (an arealist or maplist row's .label has its own in
    .label_raw, and stringdl's .prompt, msgline's .recipient, trade's
    .partner and handover's .npc theirs in .prompt_raw, .recipient_raw,
    .partner_raw and .npc_raw); a segment's .color is 'default', 'green' or
    'color<n>'.

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

    options() on a yes/no window is {name, id, text, segments, undecoded,
    default, cancellable, hold} while it asks: the question, default true
    when the game offers yes first, hold true for a question the player
    confirms by holding the key. answer(name, true) says yes and
    answer(name, false) no, as the game's own confirm on that row does, a
    held confirm included; cancel(name) is the game's own cancel and is
    refused for a question that cannot be canceled. When the reply asks
    another question, that one is a new prompt with a new id. The resyn
    windows are one prompt under the client's language: only the one the
    game opens answers. block() on merityn, shopbuy or shopsell refuses the
    questions the game asks in it: no reply ever reaches the asker, and
    whatever it set up before asking stays as it was.

    options() on a number box -- moneyctr, which asks for gil, or
    itemctrl, which asks how many of an item -- is {name, id, value, min,
    max, blocked} while it asks: value the number it shows, min..max the
    numbers it takes, blocked true when a block kept its window from
    opening. An item count asked for an item the player picked from the
    inventory, to hand over, trade, send or use in a synthesis, carries
    that item's id in .item. answer(name, number) gives the window that
    asked that number, as the box's own confirm does, and is refused for
    anything but a whole number from min to max; cancel(name) is the box's
    own cancel. A block keeps the question waiting with no window: it
    starts with its blocked event and a pending event, .what the box's name
    and .pending true; pending() lists it under that name until it ends;
    and its end is a pending event with .pending false, which every handle
    gets but the one whose reply ended it: another addon answered it, or
    the game dropped it as the window that asked closed or moved on.
    Blocked, that window stays the player's to use, so an answer is refused
    while its cursor is off the row the question was asked from or it is
    busy with another step; a cancel is not. An auction bid takes no answer
    while blocked, only a cancel: a new search can replace the item it asks
    about. A blocked question from a window the library does not know takes
    neither answer nor cancel. A box with its window up has no pending
    event: opened and closed describe it.

    options('dead') is {name, id, seconds, askable} while the KO window is
    up: seconds is the time its countdown shows, and askable whether
    answer('dead') can ask now (not while comyn has a question pending).
    answer('dead'), with no value or true, is the window's own confirm: the
    game asks its question through comyn, which answer('comyn', true) or
    answer('comyn', false) then answers. cancel('dead') is refused: the
    window has no way out.

    sortyn asks the player to confirm dropping an item or sorting a bag or
    a spell list, and evitem01 to confirm using a key item. options() on
    either is {name, id, text, segments, undecoded, default, cancellable,
    blocked}, the question in .text and, for sortyn, its heading in
    .title; default is true when the window opens on yes. answer(name,
    true) and answer(name, false) reply as the window's own confirm on that
    row does, and cancel(name) as its cancel does. confyn asks whether to
    restore a config page's defaults: options('confyn') is {name, id,
    default}, answer('confyn', true) restores them and answer('confyn',
    false) leaves them; nothing in the game waits on it, and it has no
    cancel.

    bankmenu asks which Mog Safe to use. options('bankmenu') is {name, id,
    containers, blocked}, each container {id, available}: 1 the Mog Safe,
    9 the Mog Safe 2, which is not available until the player has one.
    answer('bankmenu', id) picks a container and cancel('bankmenu') backs
    out. maplist lists the maps the map window can show. options('maplist')
    is {name, id, rows, top, level, blocked}, each row {text, value, kind}
    with .label or .count beside it as the window draws them: kind 'map'
    for a map, with its .zone and .map, 'list' for a row that opens a list
    of its own and 'close' for the row that closes the list; level is 'top'
    at its first level (top true), 'region' in a region's list and 'zone'
    in one zone's maps. answer('maplist', value) confirms that row as the
    player's key does: a 'list' row opens its list. cancel('maplist')
    closes the list at its first level and goes back up a level below it.
    Each level is a prompt of its own, with its own id, so a reply made
    with the id of the level before is refused.

    gmtell carries a GM's question. options('gmtell') is {name, id, gm,
    title, options, cancellable}: the GM's name, the question, and each
    answer the GM offers as {text, segments, undecoded, value}.
    answer('gmtell', value) sends that answer and cancel('gmtell') declines.
    mp_dead waits for the player's choice after a Monstrosity KO.
    options('mp_dead') is {name, id, options, seconds}: its two rows as
    {text, segments, undecoded, value}, values 1 and 2, and the time its
    countdown shows; when that runs out the game chooses 2 itself.
    answer('mp_dead', value) chooses; it has no cancel. ok is the game's
    notice that waits for the player, the one it shows when the connection
    to the server is lost. options('ok') is {name, id, text, segments,
    undecoded, blocked}, and answer('ok'), with no value or true,
    acknowledges it; it has no cancel.

    rem4line and rem4li2 show an NPC's dialogue, each the same message
    from its own copy: rem4line over the chat log, rem4li2 over the second
    one. The two names are one dialogue: hiding or unhiding either hides
    or unhides both, a block of either is refused for both, and answer and
    cancel through either reach both. Each name still reports its own box
    in options(), info() and its events, and each box moves with its own
    chat log, a move or a resize placing it alone. options(name) is nil
    while the box shows none; else {name, id, text, segments, undecoded,
    waits, seconds, auto, more, queued, cancellable}: the page the box
    shows last, its lines joined by '\n' and the NPC's name on the first
    page of a message; waits 'key', 'timer' or 'key_or_timer', what moves
    the page on, and seconds the time a timed page has left; auto true
    while the NPC's script confirms each page itself; more true while a
    page of this message follows; queued the messages waiting behind it;
    cancellable whether the event allows a cancel. answer(name), with no
    value or true, shows the next page, one page as the player sees it
    (both boxes move on, as with the player's key), and is refused on a
    page that goes on by itself; cancel(name) is each box's own cancel,
    made on both, refused while the event allows none, and ends the NPC's
    event. Both names report the same id. The player's keys and clicks
    reach the box the game makes active (usually rem4li2), and are kept off
    it while the two are hidden, though they move both boxes on.

    mogdoor asks where to go from the Mog House's door. options('mogdoor')
    is {name, id, title, level, options, areas, blocked}: level 'top' or
    'areas', the rows the window shows; options, that level's rows, each
    {text, kind, value, enabled}: kind 'stay' (cancel('mogdoor') closes the
    window, whichever level it shows, as the game's Escape does at the top),
    'back' (value 0, the area the player came from), 'floor' (value 125 or
    126, the Mog House's other floor) and 'areas' (the row that opens the
    list of areas); areas, the list, each {text, kind, value, zone,
    enabled}, kind 'area' or 'garden' (value 127, the Mog Garden).
    answer('mogdoor', value) takes any enabled value at either level, as
    the game's confirm on that row does; the game may still refuse, saying
    why itself. A block keeps the door's question waiting with no window
    (a blocked event, then a pending event, as for the windows above)
    until the player leaves the Mog House or uses the door again.

    The text-entry windows take what the player would type, the game's own
    Shift-JIS bytes (convert from UTF-8 with windower.to_shift_jis), each
    refused with the game's own limit and why. stringdl is the outgoing
    box's recipient: options('stringdl') is {name, id, mode, prompt} with
    mode 'name' (and min_length, max_length) while it asks the name, and
    'confirm' while it asks yes or no about a delivery slot; .prompt is the
    line over the field. answer('stringdl', name) takes 3 to 15 letters
    (the game sets their case), answer('stringdl', true or false) the
    confirm, and cancel('stringdl') backs out as the window's own cancel
    does. comment holds the bazaar, search and linkshell messages:
    options('comment') is {name, id, mode, lines}, mode 'bazaar', 'view'
    (another player's, which takes no answer), 'search' (with .category)
    or 'linkshell', lines the message's three, each {text, raw}.
    answer('comment', {line1, line2, line3}) takes up to three strings of
    at most 40 bytes, the lines left out empty, and sends the message as
    the window's close does; comment has no cancel: its Escape sends too.
    msgline writes a PlayOnline friend message: options('msgline') is
    {name, id, mode, max_length, recipient, blocked}; answer('msgline',
    text) sends it (299 bytes at most, 15 in modes 2 and 7; empty sends
    nothing, so cancel instead), refused while the last one is still on its
    way, and cancel('msgline') closes it. scname is the player search's
    name: options('scname') is {name, id, max_length, text}; answer('scname',
    name) takes at most 15 letters and starts the search, cancel('scname')
    closes it. link11 makes a new linkshell for the row link5 has selected:
    options('link11') is {name, id, max_length, text, color}, the name it
    holds in .text, color {r, g, b}; answer('link11', {name = name, color = {r, g, b}}) takes 1 to 19
    letters and colors 0 to 15 (color left out keeps the window's), and
    cancel('link11') backs out to link6. A block keeps msgline's message
    waiting with no window, as it does a number box's question: pending()
    lists it under 'msgline' until another message is set up, it is
    answered or canceled, or msgline opens. The others ask nothing with
    their window blocked; stringdl is never blocked.

    trade is a trade with another player, handover one with an NPC.
    offer(name, item, count) puts count of the inventory item at index item
    (as windower.ffxi.get_items(0) numbers the inventory) into the first of
    the window's eight item slots that is free, offer(name, 'gil', amount)
    sets the gil offered (0 takes it out), and withdraw(name, item) takes
    that item back out. answer(name), with no value or true, is the window's
    OK; cancel(name) cancels the trade and closes it. Each is refused up
    front with the reason: a ninth item, an item already offered (another
    stack of the same item is another item), a count below 1 or above what
    the inventory slot holds, more gil than the player has, an item not
    offered, any change once the player's own OK is in, for trade more than
    one change a second and for handover more than one OK a second, the most
    the game sends: the reason says how long to wait, as
    options('trade').next_change and options('handover').next_ok do, 0 when
    it would go now. An item the game grays out in its own list for that
    window is refused up front too, 'that item cannot be traded' (for
    handover 'that item cannot be handed to an NPC'): while either window is
    open the engine runs the game's own test of every inventory item each
    frame, and options(name).gray lists the inventory indexes it grays out,
    to draw them as the game does. Should the item change between the call
    and the change, the game's test, run again as the change is made,
    refuses it by an error event. options('trade') is {name, id, partner,
    mine = {gil, items}, theirs = {gil, items}, ok_mine, ok_theirs,
    next_change, gray}, each item {slot, item, id, count}, item the
    inventory index (the partner's have none). The trade shows a change once
    the server has taken it; until then mine.items lists the item with
    pending true, and it counts as offered, but takes no withdraw.
    options('handover') is {name, id, npc, items, gil, next_ok, gray}:
    handover sends the whole offer at its OK and closes. next_change and
    next_ok are nil while the game's clock cannot be read, gray until the
    engine has run the test on this open of the window. Blocked, trade's
    opening cancels the trade, and handover never opens: the NPC trade does
    not start, and its replies are refused.

    A block keeps a question of sortyn, evitem01, maplist, ok or bankmenu
    waiting with no window, as it does a number box's: a blocked event and
    a pending event start it, pending() lists it under the window's name,
    answer() and cancel() take it as they take one in the window, and its
    end is a pending event every handle gets but the one whose reply ended
    it. sortyn's, evitem01's and maplist's end when the window that asked
    closes, evitem01's too when the key item list moves to another item,
    and maplist's when the game builds the list again; a reply that takes
    maplist to another level ends its question and starts that level's,
    with a pending event of its own. bankmenu's waits until it is answered
    or canceled, and until then the game opens no other Mog Safe picker.

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

    settings() lists every setting of the game's Config menu, each control
    the player finds on one of its pages, in the menu's order: {name, page,
    kind, value, default, min, max, choices, label}. page is the Config
    window that shows it; kind 'number', 'bool', 'choice', 'color' or 'key';
    value and default are as set_setting takes them: a whole number, true or
    false, a choice's name, a color's {r, g, b} (each 0 to 255), a
    DirectInput key code (0 to 255). min and max are the numbers a number
    or a key takes, or a color's channel; choices a choice's names in its
    page's row order; default nil where the game keeps none; label the
    game's text for it as UTF-8 (its bytes in .label_raw), nil where the
    game has none. setting(name) is one setting's value. set_setting(name,
    value) sets one as its page does, and the game applies it at once; a
    value of another type than its kind takes raises, and a value outside
    its range, an unknown name, or a set while the player is not in the game
    is refused. A page that edits a copy of its settings and writes the copy
    back as it closes (cfilter, fxfilter, conf11s, k1assign and k2assign, and
    conf3win for character_information) would put that copy over a set made
    while it is open: such a set is refused by an error event. The two key
    pages share one copy of the keys, so a key set is refused while either
    is open. The game saves the values as the player leaves the game, as with
    its own pages.

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
                     page = true, error = true, pending = true, resync = true}
local event_list = 'opened, closed, covered, uncovered, blocked, cursor, page, error, pending or resync'

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

-- Each setting's label, the game's text, as UTF-8 to draw, its bytes as
-- .label_raw.
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

for _, verb in ipairs({'hide', 'unhide', 'close'}) do
    Handle[verb] = window_call(verb, verb .. '(name)')
end

-- open(name[, argument]) and open_set(set[, argument]): an argument is a
-- string, a number, or a table of named fields, each a string or a number.
-- It reaches the engine as one string: a whole number as its digits, any
-- other in decimal to 17 places, a table as key=value pairs joined by ';' in
-- key order, which is why a string may hold neither '=' nor ';'. The engine
-- says which windows take a fraction.
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

for _, verb in ipairs({'list', 'opened', 'focused', 'remembered', 'groups', 'pending', 'rects', 'macros',
                       'settings'}) do
    Handle[verb] = read_call(verb)
end

-- handle:setting(name): the setting's value.
function Handle:setting(name)
    check_self(self, 'setting')
    need(type(name) == 'string', 'setting(name): name must be a string')
    if not self.native then
        return nil, released
    end
    return call(self, 'setting', name)
end

-- handle:set_setting(name, value): the type of value each setting takes is
-- the engine's to say, by its kind.
function Handle:set_setting(name, value)
    check_self(self, 'set_setting')
    need(type(name) == 'string', 'set_setting(name, value): name must be a string')
    if not self.native then
        return nil, released
    end
    return call(self, 'set_setting', name, value)
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

-- handle:offer(name, item, count) or handle:offer(name, 'gil', amount).
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

-- handle:withdraw(name, item).
function Handle:withdraw(name, item)
    check_self(self, 'withdraw')
    need(type(name) == 'string', 'withdraw(name, item): name must be a string')
    need(type(item) == 'number', 'withdraw(name, item): item must be an inventory index')
    if not self.native then
        return nil, released
    end
    return call(self, 'withdraw', name, item)
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
