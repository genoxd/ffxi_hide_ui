# ffxi_hide_ui

Hide, move, resize, open, close and answer the windows of Final Fantasy XI's
own interface, from a Windower 4 addon.

The client builds its interface out of 369 named windows. You can

- hide any window, or block it so the game never opens it
- move the windows
- resize windows (doesn't look great)
- open and close windows
- subscribe to events when the game opens, closes, covers or uncovers a window, or
  tries to open one that is blocked
- read any window: whether it is open or has focus, where it is, which row the
  cursor is on, what its elements say
- read what a prompt offers and answer it in the window's place: an NPC's
  choice list, a text entry, a party invite, the linkshell and area lists, the
  delivery box

Two things that are not windows are covered as well: the compass, which you
can hide and move like a window, and the macro keys, which you can block so
that Ctrl+number and Alt+number are free for your own binds.

This library draws nothing (shameless plug for [ffxi_world_draw](https://github.com/genoxd/ffxi_world_draw)).

Nameplates, damage numbers, the target cursor, the mouse cursor and the
loading screen's graphics are not windows, and the library does not cover them.

## Install

Copy three files into your addon's own `libs/` folder (not Windower's shared
`addons/libs/`). They are prebuilt, in the example addon's folder,
[`examples/hideuidemo/libs/`](examples/hideuidemo/libs). `hideui.lua` is the
library; the two DLLs are what it loads.

```
addons/myaddon/
├── myaddon.lua
├── data/                     only if you save a layout (see below)
└── libs/
    ├── hideui.lua
    ├── _HideUI.dll
    └── hideui_daemon.dll
```

```lua
_addon.name = 'myaddon'                 -- every addon sets this; hideui.new() reads it

local hideui = require('libs.hideui')

local ui, why = hideui.new()            -- labelled with your addon's name
if not ui then
    windower.add_to_chat(123, 'myaddon: ' .. why)   -- e.g. after a game patch
    return
end

ui:on('error', function(e)              -- anything the game refuses lands here
    windower.add_to_chat(123, ('myaddon: %s %s: %s'):format(e.verb, e.name or '', e.reason))
end)
```

`ui` is your handle. Everything you do goes through it, and unloading your
addon undoes all of it: hidden windows come back, blocked ones can open,
moved and resized windows go home. `ui:release()` does the same without
unloading. The handle carries your addon's name, which other addons see when
they ask who moved a window; `hideui.new('other')` labels it differently.

Calls that change something return `true`, or `nil` and a reason. `true`
means the change is queued for the game's next frame. If the game refuses it
then, the refusal arrives as an `error` event with `e.verb` naming the call,
which is why the handler above goes in first. Calls that read something
return what they read, or `nil` and a reason. Misuse raises at your line
instead: a wrong argument type, `ui.hide` written for `ui:hide`, an unknown
event or window name in `on`.

## Window names

Every window has a short name of the game's own, up to eight characters:
`logwindo` the chat log, `partywin` the party list, `targetwi` the target
window, `equip` the equipment window, `inventor` the inventory, `iteminfo` an
item's description, `query` every NPC choice list. To find a name, load the
example addon (see [Try it first](#try-it-first)) and open the window in
game:

```
//lua load hideuidemo
//hideuidemo events on        every window prints its name as it opens
```

## Hide and block

```lua
ui:hide('targetwi')        -- the target window is no longer drawn
ui:unhide('targetwi')      -- back

ui:block('equip')          -- the equipment window can no longer open
ui:unblock('equip')        -- it can again
```

A hidden window keeps running but takes no mouse, keyboard or gamepad
input. The one exception is typed text: a hidden text entry (`passinpu`)
can still receive characters, though not Enter or Escape. Its contents
still update, so you can read them, and it stays hidden through its closes
and opens until you unhide it. Escape does not close a hidden window;
closing it is your addon's job, with `close` or, for a prompt, `cancel`.

A blocked window never opens. The open is refused before anything is drawn,
and every addon gets a `blocked` event; `e.mine` is true for the addon
whose block it is. If the window is open when
you block it, the game's own close closes it first; a prompt that is open
when you block it (`passinpu`, `prtyjoin`, `link5`, `arealist`, the
delivery boxes) is hidden instead of closed, because closing it would strand
what the game is waiting on. `query` cannot be blocked at all.

Some windows are part of something the game is in the middle of, and
blocking them has consequences:

| window | if blocked |
|---|---|
| `query` (every NPC choice list) | `block` is refused: the game would crash. Hide it and answer it (see [Prompts](#prompts)) |
| `trade` | the trade is cancelled |
| `delivery` (outgoing delivery box) | the player cannot move until you call `ui:cancel('delivery')` and the server answers (see [Prompts](#prompts)) |
| `post1`, `post2` (incoming delivery box; `post2` is its second screen) | the session stays open until `ui:cancel('post1')` (see [Prompts](#prompts)) |
| `passinpu`, `link5`, `arealist` | the NPC's script waits until you `answer` or `cancel` it (see [Prompts](#prompts)) |
| `prtyjoin` (the party menu's invite screen) | the invite waits until you `answer` or `cancel` it |
| `mcr1pall`, `mcr2pall` (the Ctrl and Alt macro bars) | the bar no longer shows, but Ctrl+number and Alt+number still run the macros; to stop those, see [Macro keys](#macro-keys) |

Shops, the auction house (`auc1`), `inspect`, the target window, the cast bar
(`casttime`) and the map windows are fine to block. Windows in neither list
have not been checked.

## Move

```lua
ui:move('targetwi', 1500, 500)   -- top-left corner to 1500,500
ui:reset('targetwi')             -- back where the game puts it, position and size
ui:reset('targetwi', 'position') -- one aspect only; 'size' is the other
ui:reset_all()                   -- every window your addon was the last to move or resize
```

Coordinates are the game's UI coordinates: 0,0 is the top left, and
`hideui.status().ui` gives the size as `{w = 1920, h = 1080}` (see
[When it cannot work](#when-it-cannot-work)). The position lasts as long as
your addon is loaded: every time the game opens the window again, it lands
there.

`reset` touches only what your addon placed; a window another addon placed
is refused with that addon's name.

Some windows are docked: the game places them against another window and
puts them back there whenever that window moves (the target window sits on
the party list, for example). Moving a docked window undocks it.

To move a window with everything docked to it, name the group instead. There
are three, and they nest: `target_window` (`targetwi` and its sub-target
panel) sits inside `party_list` (`partywin` and the windows on it), which
sits inside `chat_log` (`logwindo` and the menus that open above it).

```lua
ui:move_group('chat_log', 16, 830)        -- the chat log, the party list and the target window with it
ui:move_group('party_list', 1700, 900)    -- the party list, the target window, the sub-target panel
ui:move_group('target_window', 1700, 980) -- the target window and the sub-target panel
```

Both chat logs and the party list grow upward: their frame's top moves as lines
or members come and go. `move` and `move_group` still take the frame's top-left
for them, as for every window. If the group's window is closed when you call
this, the move happens when the game opens it. `ui:reset_group('chat_log')`
undoes a group move.

## Resize

```lua
ui:resize('partywin', 3)        -- the party list at 3 rows (1-6)
ui:resize('playermo', 4)        -- the action menu at 4 rows (1-10)
ui:resize('targetwi', 200, 60)  -- any window, to a width and height (same units as move)
```

Rows use the game's own layouts. Three windows have them: `partywin` 1-6,
`playermo` 1-10 and `mp_pmode` (the Monstrosity action menu) 1-8. A width
and height only change the frame; the contents do not reflow. A size is put
back every time the window opens, and `reset` undoes it. The party list
re-sizes itself on a roster change and the action menu on a row change;
after that the size is theirs until the next open. The item descriptions
(`iteminfo`, `itemxinf`) size themselves every frame, so a resize of them
does not last.

## Open and close

```lua
ui:open('equip')
ui:close('equip')
```

`open` works where the game itself would open that window at that moment. The
equipment window opened outside the main menu closes again at once, and you see
`opened` then `closed`. The prompts listed under [Prompts](#prompts) can only
be opened by the game; use `cancel` to close them.

## The compass

The compass at the lower left, with the Vana'diel clock and the weather
icon, is not one of the 369 windows: the client draws it separately. The
library covers it anyway, under the name `compass`, and for these four
calls it behaves like a window:

```lua
ui:hide('compass')
ui:unhide('compass')
ui:move('compass', 200, 900)    -- top-left corner of its box
ui:reset('compass')
```

Its box is 88 by 42 and normally sits just above the chat log;
`ui:info('compass').rect` tells you where it is right now. Left alone, the
game keeps it above the chat log, moving it whenever the log grows or
shrinks, and shows it again after a cutscene or when you close the map.
Once you move it, it stays where you put it until you reset it.

The other calls are simpler for the compass than for a window. `block` and
`close` do the same thing as `hide`, and `unblock` and `open` the same as
`unhide`: the game shows and hides the compass by itself, so there is
nothing else to block or close. `resize` is accepted and does nothing,
because the compass has no rows or frame to size. `info('compass')`
reports no cursor and no elements, since it has neither. The `closed`
event fires when the game hides the compass for a cutscene, and `opened`
when it brings it back. The clock inside it is switched with the game's
own `/clock on` and `/clock off`; the library leaves it alone.

## Macro keys

Ctrl+1 through Ctrl+0 and Alt+1 through Alt+0 run the game's macros.
Blocking the macro bar windows does not stop that, because the bar is only
a display and the game handles the keys separately. To take those keys for
your own Windower binds, block the macros themselves:

```lua
ui:block_macros()     -- Ctrl+number and Alt+number no longer run macros
ui:unblock_macros()
ui:macros()           -- { blocked = true, blocked_by = { 'myaddon' }, mine = true }
```

While the block is on, pressing Ctrl or Alt shows no bar, and Ctrl+number
and Alt+number do nothing. If a bar is showing when you call
`block_macros`, it closes. The block stays on until you call
`unblock_macros` or your addon unloads. If two addons have blocked the
macros, they stay blocked until both have unblocked them.
`hideui.status().macros_blocked` tells you whether any addon has a block
on.

## Events

```lua
ui:on('opened', 'equip', function(e) print('equip opened') end)   -- one window
ui:on('closed', function(e) print(e.name .. ' closed') end)      -- every window

local fn = ui:on('opened', 'equip', function(e) end)
ui:off('opened', 'equip', fn)                                    -- remove one
ui:off('opened')                                                 -- remove every opened handler
```

Every event carries `e.name` except `pending` and `resync`.

| event | when |
|---|---|
| `opened` | the window is open now: the game opened it, or re-opened it while it was already open, so never count opens against closes |
| `closed` | the window is gone |
| `covered` | another window went over it; it is still open underneath |
| `uncovered` | it came back out |
| `blocked` | the game tried to open a window some addon blocked |
| `error` | the game refused something you asked for, or one of your own callbacks raised (`e.verb` is `'callback'`): `e.verb`, `e.name`, `e.reason` |
| `cursor` | the game's cursor in an open window moved to row `e.row`; for `query` and `arealist`, `e.row` is the index into the list `options()` returns |
| `pending` | a party invite or a delivery-box session started or ended (see [Prompts](#prompts)): `e.what` is `'invite'` or `'post'`, `e.pending` true or false |
| `resync` | the library keeps the last 4096 events for you; if your addon falls further behind than that, it gets this single event instead of the ones it missed, with `e.dropped` set to how many. Re-read `opened()`, `info()` and `pending()` to catch up |

Callbacks run on the frame after the event, so a window may already be gone
when `opened` reaches you; check `ui:info(name).open`. A window already
open when your addon loads does not send `opened`; check on load. A callback
that raises never stops the others. Its first raise comes back as an `error`
event and later ones are only counted; `hideui.debug(true)` prints them.
Every event table has `e.event`, so one handler can serve several events.

## Read a window

```
local p = ui:info('targetwi')     -- nil and a reason for a name that is no window
p.open                -- true/false
p.hidden, p.blocked   -- by any addon
p.rect.x, p.rect.y, p.rect.w, p.rect.h   -- while open
p.cursor              -- the row the game's cursor is on, 1-based, while open; for query and arealist, the index into their list
p.top                 -- query and arealist: the index of the first row on screen (query shows three, arealist five)
p.focused             -- has the keyboard
p.elements            -- while open: a list of {type = 'item', x = 44, y = 836, w = 132, h = 16, text = 'Yes'};
                      --   type is 'frame', 'item', 'cursor' or 'other'; the other fields only where the part has them
p.hidden_by, p.blocked_by   -- names of the addons holding a hide or a block on it
p.blockable           -- false only for query
p.resize              -- {holds = 'reopen'|'trigger'|'frame'|'none', min_rows, max_rows}: how long a size lasts, and the row range if it has one
p.memory              -- the remembered position and size, as remembered() lists them
p.covered             -- open but under another window
p.docked              -- the game keeps it against another window
p.detail              -- internals; present only while hideui.debug(true) is on (see When it cannot work)

ui:opened()           -- { 'logwindo', 'partywin', ... }: every window open now
ui:focused()          -- the name of the window with the keyboard, or false
ui:list()             -- every window's open/hidden/blocked/moved/resized state, keyed by name
ui:groups()           -- { chat_log = {anchor, anchor_open, origin, members, waiting}, ... }
ui:remembered()       -- { targetwi = {position = {x, y, owner, mine}, size = {rows | w, h, owner, mine}}, ... }
ui:rects()            -- { logwindo = {x = 16, y = 898, w = 1774, h = 166}, ... }: every open window's frame
```

## Prompts

Some windows are the game asking the player something: an NPC's choice list,
a text entry, a party invite. If you hide one to draw your own, you also have
to answer it, or the player is stuck.

```lua
local q = ui:options(name)    -- what the prompt is offering; nil once it is gone
ui:answer(q, value)           -- the player's choice
ui:cancel(q)                  -- the player's cancel
```

`q` names the exact prompt you read, so a reply that arrives after the game
has replaced it is refused instead of landing on the next one.
`ui:answer(name, value)` and `ui:cancel(name)` also work when you don't
care which.

### The NPC choice list

Every list of choices an NPC offers is the window `query`: yes/no, the home
point menus, a treasure chest's items. Hide it, draw the options your own
way, and answer with the one the player picks:

```lua
ui:hide('query')                                 -- the game's list is never drawn

-- draw_my_list and hide_my_list are yours to write
local q, pick                                    -- the list on screen, and the option the player is on

local function show()
    q = ui:options('query')                      -- what the NPC is asking; nil if it closed already
    pick = 1
    if q then draw_my_list(q.title.text, q.options, pick) end
end

ui:on('opened', 'query', show)
if ui:info('query').open then show() end        -- already open when the addon loaded

ui:on('closed', 'query', function()
    q = nil
    hide_my_list()
end)

windower.register_event('keyboard', function(dik, down)   -- dik: the DirectInput key code
    if not q or not down then return end
    if dik == 203 and pick > 1 then                           -- Left
        pick = pick - 1; draw_my_list(q.title.text, q.options, pick)
    elseif dik == 205 and pick < #q.options then              -- Right
        pick = pick + 1; draw_my_list(q.title.text, q.options, pick)
    elseif dik == 28 then                                     -- Enter
        ui:answer(q, q.options[pick].value); q = nil          -- one answer per list
    elseif dik == 1 and q.cancellable then                    -- Escape
        ui:cancel(q); q = nil
    end
end)
```

The hidden list ignores every key and click, so the layout and the keys are
entirely yours; this one runs left to right. The keys still reach the game,
which does nothing with them while a hidden list is up. `ui:options('query')`
returns:

```lua
{
    name = 'query',
    id = 12,                        -- this particular open of the prompt
    title = { text = 'Teleport where?', raw = '...' },
    cancellable = true,             -- whether backing out is allowed here
    options = {                     -- in the game's order
        { text = 'Nowhere.',            value = 1 },
        { text = 'Home Point #1 (E).',  value = 2 },
        { text = 'Home Point #4.',      value = 5 },   -- values skip where the game left options out
    },
}
```

Answer with an option's `value`, never its position: "Yes" is not always
1.

Every string the library hands you is UTF-8: option and title text, element
captions, linkshell names, the inviter of a party invite. That is the form
Windower's text objects draw. The game's own bytes sit beside each as `raw`
(`label_raw` for an area list label); that is the form the game's chat
wants, so print `raw` with `windower.add_to_chat`, never `text`. A
character the library cannot decode comes back as `?`, and `undecoded` on
the title and on each option counts them. Where the game colors part of a
title or option, it also carries `segments`, a list of `{text, color}` runs
with `color` one of `'default'`, `'green'`, or `'color<n>'` for a color
without a name.

### The other prompts

| window | `ui:options(name)` | `ui:answer(name, ...)` | `ui:cancel(name)` |
|---|---|---|---|
| `passinpu` text entry | `{max_length = 16}` | the text, up to `max_length` bytes (plain ASCII, or `windower.to_shift_jis` for anything else; longer is refused) | no text |
| `prtyjoin` party invite | `{inviter = 'Somebody', alliance = false}` | `true` accept, `false` decline | decline |
| `link5` linkshell list | `{slots = {{slot = 1, name = 'MyShell'}, ...}}` | a `slot` from the list | close, no choice |
| `arealist` area list | `{name, id, pending, mode, level, rows = {{text = 'Bastok Mines', id = 234, kind = 'zone'}, ...}}`: every row the game shows, in order. `kind` is `zone` (an `id` from Windower's `res.zones`), `region` (a heading the player can open; `level` is 0 at the top and that region's `id`, negated, inside one), `current_area`, `current_region`, `all`, or `other`. A zone row carries `label`, a region row `count`. `mode` says who opened it: 1 or 2 an NPC, otherwise a menu of the player's own, which is read-only here | a zone `id` from the rows, when an NPC is waiting (`mode` 1 or 2). A blocked list has no rows and `pending = true`; then any zone id is accepted | closes the list with no zone |
| `delivery` outgoing box | refused | refused | end the session |
| `post1`, `post2` incoming box | refused | refused | end the session |

With a box open, `cancel` does what the box's own closing control does and
the windows close when the server answers, a moment later.

A party invite shows no window until the player opens the party menu, and
a delivery-box session can be up with its windows blocked. The `pending`
event tells you when either appears, and `ui:pending()` shows what is
waiting; its `invite` and `post` tables go straight into `answer` and
`cancel`:

```lua
local w = ui:pending()
if w.invite then ui:answer(w.invite, true) end     -- w.invite.inviter, w.invite.alliance
if w.post then ui:cancel(w.post) end               -- w.post.box is 'delivery' or 'post1' (post2 is post1's second screen)
```

## Save a layout

Positions and sizes last only while your addon is loaded. To bring them
back next time, ask for them, save them, and apply them on load:

```lua
-- on load
local config   = require('config')
local settings = config.load({layout = {}})
ui:apply(settings.layout)           -- a closed window takes its place when it opens
```

```lua
-- after your addon's moves and resizes, and in its unload handler
settings.layout = ui:layout()       -- every position and size your addon set, group moves included
config.save(settings, 'all')
```

`ui:layout()` returns a plain table of what your addon placed, plus the entries
an `apply` could not place, which the config library can save as is. It is
yours alone: another addon moving the same window later does not change it. It
records the UI size, and `ui:apply()` refuses a layout saved at a different
size. The config library writes `data/settings.xml`, so your addon needs a
`data/` folder. If some entries cannot be applied, `ui:apply()` returns `nil`,
a reason and the list of those entries, and applies the rest.

## More than one addon

Several addons can use the library at once. Hides and blocks add up: a
window hidden or blocked by two addons stays that way until both let go,
and the same goes for macro blocks. Moves do not: the library
keeps one position per window, so a window moved by two addons sits where
the later move put it, and when that addon unloads the window goes back to
where the game puts it, not to the earlier addon's spot. A `blocked` event
says whose block it was: `e.mine` is true when it is yours, and `e.by` lists
every addon holding one.

```lua
local p = ui:info('targetwi')
p.hidden_by     -- { 'myaddon', 'otheraddon' }: who is hiding it
```

## When it cannot work

`require('libs.hideui')` raises when `_HideUI.dll` is missing or will not
load; wrap it in `pcall` if your addon should survive that. `hideui.new`
returns `nil` and a reason when the library cannot install: the daemon file
is missing, another addon loaded an older copy first, or a game patch
changed the code it relies on. Show the reason and carry on without the
handle.

```lua
hideui.version()      -- 'hideui 0.9.0'
hideui.debug(true)    -- print this addon's library failures to chat while you develop; otherwise it prints nothing
hideui.status()       -- .ok (installed), .ui.w, .ui.h, .dropped (every addon's), .macros_blocked, and .hidden, .blocked, .moved, .resized: names
ui:status()           -- the same, with .dropped for this handle only
```

## Try it first

[`examples/hideuidemo`](examples/hideuidemo) turns the library's calls into
commands, so you can find a window's name and try things before writing code.
Copy the folder, `data/` included, into Windower's `addons/`, then:

```
//lua load hideuidemo
//hideuidemo events on          print every event; open any window in game to see its name
//hideuidemo hide logwindo
//hideuidemo unhide logwindo
//hideuidemo block equip
//hideuidemo move targetwi 1500 500
//hideuidemo group chat_log 16 830
//hideuidemo resize partywin 3
//hideuidemo reset targetwi
//hideuidemo reset targetwi position
//hideuidemo resetgroup chat_log
//hideuidemo rects
//hideuidemo info equip
//hideuidemo opened
//hideuidemo options query      with an NPC's list open
//hideuidemo answer query 2
//hideuidemo cancel query
//hideuidemo layout          save this addon's layout to data/settings.xml
//hideuidemo apply           put a saved layout back
//hideuidemo status
//hideuidemo debug on        print the library's own failures
//hideuidemo help
```

`//hideuidemo list` prints every name; `//hideuidemo list item` those containing
"item".

## Updating the library in your addon

Whichever addon loads first puts its copy of the library in charge, and
every addon loaded after it uses that copy, until all of them have unloaded.
That is fine when your copy is older than the one in charge. When yours is
newer and needs a call the older copy does not have, `new` returns `nil`
and names the addon that should be updated. A field the older copy does
not know is missing from what you read.

To replace `_HideUI.dll`, unload every addon that uses the library, wait a
few seconds, then copy the new file in. Never write over it while it is
loaded. `hideui_daemon.dll` stays loaded until the game closes, so replace
that one with the game closed.

## Under the hood

The library finds the client's window table and the routines it needs in
the running game, and hooks eight of them: the ones that open, show and
close a window, run the UI each frame, decide whether the mouse is in a
menu, pass a key to a window, draw the compass and let macro keys run. It
makes every change from inside the game's own thread. If a game patch changes any of that, nothing installs
and `new` returns the reason.

Building it: [`engine/README.md`](engine/README.md) and
[`daemon/README.md`](daemon/README.md).

## All window names

Every name the library accepts, with what it is where known. The
descriptions come from the window's own captions or the menu it opens from.
A blank means it has not been identified yet; the name is still valid.

| name | window |
|---|---|
| `ability` | job ability list |
| `abimenu` | abilities menu (traits, abilities, weapon skills) |
| `abiselec` | ability category list |
| `abisortw` | ability sort menu |
| `acoption` | chat mute list option |
| `alarm` | party invite alert settings |
| `allied` | allied notes total on the campaign map |
| `arealist` | area list (quest menu, and the NPC prompt) |
| `armsort` | auction house sort menu (armor) |
| `auc1` | auction house |
| `auc2` | auction house category menu |
| `auc3` |  |
| `auc4` | auction house: your listing actions |
| `aucammo` |  |
| `aucarmor` | auction house: armor categories |
| `aucfood` | auction house: food categories |
| `auchisto` | auction house sales history |
| `aucitem` | auction house: misc categories |
| `auclist` | auction house item list |
| `aucmagic` | auction house: scroll categories |
| `aucmater` | auction house: material categories |
| `aucmeals` | auction house: meal categories |
| `aucweapo` | auction house: weapon categories |
| `automato` | automaton customization |
| `bank` | Mog Safe contents |
| `bankmenu` | furnishing source selector |
| `bazaar` | bazaar setup |
| `beseige1` | Besieged status (region menu) |
| `beseige2` | enemy base info on the Besieged map |
| `blklist` |  |
| `blkmain` |  |
| `bluehelp` | blue magic help |
| `bluepoin` | blue magic points |
| `bluequip` | blue magic equip list |
| `bluesibo` | linkshell item selector |
| `bluinven` | blue magic set list |
| `blusortw` | blue magic sort menu |
| `btlskill` | combat skills (status menu) |
| `buff` | status effect icons |
| `camparea` | linkshell item selector (second form) |
| `campresu` |  |
| `campsan` |  |
| `casttime` | cast bar |
| `cfilter` | chat filter settings |
| `charlnk` |  |
| `chatctrl` |  |
| `chdummy` | loading screen "Downloading Data" |
| `chfwin` |  |
| `chmkface` |  |
| `chmkhair` |  |
| `chmkjobs` |  |
| `chmkname` |  |
| `chmkpass` |  |
| `chmkrace` |  |
| `chmkserv` |  |
| `chmksize` |  |
| `chmktown` |  |
| `chocobor` |  |
| `chswin` |  |
| `cmbhlst` | synthesis history list |
| `cmbhwin` | synthesis favorite toggle |
| `cmbmenu` | synthesis menu |
| `cnqframe` | nation rankings on the conquest map |
| `cnttime` |  |
| `colopoin` | Bayld total on the colonization map |
| `colorank` |  |
| `coloresu` |  |
| `comgenre` | search comment category |
| `comment` | search comment editor |
| `commenu` | friends and emotes menu |
| `compass` | the compass with the clock and weather; not a window, see [The compass](#the-compass) |
| `comyn` | confirm yes/no |
| `conf11l` | log routing: chat |
| `conf11m` | log routing menu |
| `conf11s` | log routing: system |
| `conf12wi` | effects settings |
| `conf13wi` | software keyboard settings |
| `conf1win` |  |
| `conf2win` | gameplay settings |
| `conf3win` | misc settings |
| `conf4` | word filter settings |
| `conf5m` | log window settings menu |
| `conf5w1` | log window 1 settings |
| `conf5w2` | log window 2 settings |
| `conf5win` | log window layout |
| `conf6win` | graphics settings |
| `conf7` | mouse and camera settings |
| `configwi` | config menu |
| `conftxtc` | text color settings |
| `confyn` | config confirm yes/no |
| `conq3pts` | conquest points on the conquest map |
| `conq6sta` |  |
| `conquer1` | regional standing on the conquest map |
| `dbdelsel` | a debug window; never opens in play |
| `dbnamese` |  |
| `dead` |  |
| `delivery` | outgoing delivery box |
| `emote` | emote list |
| `equip` | equipment window |
| `eventtim` | event timer |
| `evitem` | a status menu screen |
| `evitem01` | key items |
| `faqmain` | help: FAQ |
| `faqsub` | help: FAQ topic |
| `fep` |  |
| `feppalle` |  |
| `flistmai` | friend list menu |
| `flmes` |  |
| `fp_cat` |  |
| `fp_cat2` |  |
| `fp_info` |  |
| `friend` | friend list |
| `fulllog` |  |
| `fxfilter` | effects filter |
| `gaugewin` | HP/MP/TP block shown while engaged |
| `gift` |  |
| `gmtell` | GM tell window |
| `guide00` | help guide |
| `guide01` | help guide |
| `guildsho` | guild shop |
| `handover` | trade with an NPC |
| `helpwind` |  |
| `hn1blank` |  |
| `hn1exist` |  |
| `hn2blank` |  |
| `hn2exist` |  |
| `hn3blank` |  |
| `hn3exist` |  |
| `hnbackwi` |  |
| `hncansel` |  |
| `hnhead` |  |
| `hnnaming` |  |
| `hnwarnin` |  |
| `hnyesno` |  |
| `holdtime` |  |
| `imperial` |  |
| `inline` | text entry line (names typed in menus) |
| `inspect` | inspect window |
| `inventor` | inventory list |
| `itemctrl` | item action menu |
| `iteminfo` | item description |
| `itemxinf` | item description, extended |
| `itemyinf` |  |
| `itmsort2` | item sort menu |
| `itmsortw` | storage sort menu |
| `itmstora` |  |
| `iuse` |  |
| `jbpcat` | job points: job list |
| `jobchang` | job change menu |
| `jobcsel` |  |
| `jobcselu` | job selection list |
| `joblevel` | job levels (status menu) |
| `k1assign` |  |
| `k2assign` |  |
| `keylayou` |  |
| `keypad` |  |
| `keypad2` |  |
| `keypad3` |  |
| `keypadus` |  |
| `keyselec` |  |
| `level` | a status menu screen |
| `levelsyn` |  |
| `levmeri2` | job points (status menu) |
| `levmerit` | merit points (status menu) |
| `link1` | linkshell: name it or dispose of it |
| `link10` | linkshell member: send a tell |
| `link11` | linkshell: name and color |
| `link12` | linkshell member list |
| `link13` | linkshell item: activate or dispose |
| `link2` | linkshell: members, activate, create a linkpearl |
| `link3` |  |
| `link4` |  |
| `link5` | linkshell list (main menu, and the NPC prompt) |
| `link6` |  |
| `link7` |  |
| `link8` |  |
| `link9` |  |
| `lnowin` |  |
| `loby1win` |  |
| `loby2win` |  |
| `lobycwin` |  |
| `lobyhelp` |  |
| `logwin2` | second chat log |
| `logwindo` | chat log |
| `loot` |  |
| `lootope` |  |
| `lscswin` |  |
| `macro` | macro editor |
| `magic` | spell list |
| `magselec` |  |
| `map0` | map menu |
| `mapframe` | frame of the map window |
| `maplist` | map list (other areas) |
| `mapscan` | wide scan actions |
| `mapv2` | map marker actions |
| `mapv3` | map marker list |
| `masterle` |  |
| `mcr1edit` |  |
| `mcr1edlo` |  |
| `mcr1long` |  |
| `mcr1pall` | Ctrl macro bar |
| `mcr2edit` |  |
| `mcr2edlo` |  |
| `mcr2long` |  |
| `mcr2pall` | Alt macro bar |
| `mcrbedit` | macro button edit |
| `mcres20` | equipment set list |
| `mcresed` | equipment set editor |
| `mcresmn` | equipment set menu |
| `mcrledit` | macro line edit |
| `mcrmenu` | macro menu |
| `mcrmogc` | storage selector (inventory, Mog Safe, ...) |
| `mcrselec` | macro palette selection |
| `mcrselop` | macro book options |
| `menuwind` | main menu |
| `merit1` | merit points menu |
| `merit2` | experience or limit points mode |
| `merit2ca` | merit point category list |
| `merit3` | merit point adjust menu |
| `meritcat` | merit point categories |
| `meritinf` | merit points info |
| `merityn` | merit point confirm |
| `mes1rcv` |  |
| `mes2frnd` |  |
| `mgcmenu` | magic menu |
| `mgcskill` | magic skills (status menu) |
| `mgcsortw` | magic sort menu |
| `miss00` | mission list |
| `missionm` | missions menu |
| `mnstorag` | equipment source selector (inventory, wardrobes) |
| `mogcont` | Mog House contents menu |
| `mogdoor` |  |
| `mogext` |  |
| `mogpost` | delivery box menu |
| `money` | gil panel |
| `moneyctr` | gil amount entry |
| `mount` | mount list |
| `mp_abisc` |  |
| `mp_cstm` |  |
| `mp_dead` |  |
| `mp_invnt` |  |
| `mp_kncst` |  |
| `mp_mnabi` |  |
| `mp_mncst` |  |
| `mp_mnnm` |  |
| `mp_mntp2` |  |
| `mp_pmode` | action menu while in Monstrosity |
| `mp_stat` |  |
| `msgline` |  |
| `msglist` | PlayOnline messages |
| `myroom` | Mog House menu |
| `nation1` |  |
| `nation2` |  |
| `nation3` |  |
| `netbar` |  |
| `netstat` |  |
| `netwait` |  |
| `ok` |  |
| `olstat` | online status |
| `oplev` |  |
| `partywin` | party list |
| `passinpu` | text entry prompt |
| `persona` | status window |
| `playermo` | action menu |
| `plnt1` |  |
| `plnt2` |  |
| `plnt3` |  |
| `plnt4` |  |
| `plnt5` |  |
| `plnt6` |  |
| `post1` | incoming delivery box |
| `post2` | incoming delivery box actions |
| `profile` | profile (status menu) |
| `prty1` | party menu |
| `prty2` | party and alliance member row |
| `prty3` | party and alliance member row |
| `prty4` | party and alliance member row |
| `prty5` | party and alliance member row |
| `prty5de` |  |
| `prty5fr` |  |
| `prty5us` | party language |
| `prty7` | party and alliance member row |
| `prty8` | party and alliance member row |
| `prtyjoin` | party invite screen (party menu) |
| `prvdungp` |  |
| `prvdunsi` |  |
| `ptc10che` |  |
| `ptc1prog` |  |
| `ptc2warn` |  |
| `ptc3bar1` |  |
| `ptc4bar2` |  |
| `ptc6yesn` |  |
| `ptc7vup` |  |
| `ptc8lice` |  |
| `ptc9dele` |  |
| `ptcbgwin` |  |
| `pupequip` | automaton equip menu |
| `pupsibor` | attachment element filter on the automaton equipment screen |
| `query` | NPC choice list |
| `quest00` | quest and mission list |
| `quest01` | quest and mission list |
| `race1` |  |
| `race2` |  |
| `race3` |  |
| `race4` |  |
| `race5` |  |
| `race6` |  |
| `race7` |  |
| `race8` |  |
| `racecard` |  |
| `raid1` |  |
| `raid2` |  |
| `ranking` |  |
| `region` | region balance of power |
| `rem4li2` | opens with rem4line during NPC dialogue |
| `rem4line` | confirm prompt (yes/no) |
| `resyn` |  |
| `resynde` |  |
| `resynfr` |  |
| `resynus` |  |
| `rmlo1` |  |
| `rmlo2` |  |
| `rmlo4` |  |
| `rmpost` |  |
| `roomlist` |  |
| `scanlist` | wide scan list |
| `scchar` | search by level |
| `sccomite` | search comment: items |
| `sccomls` | search comment: linkshell |
| `sccompar` | search comment: party |
| `scmlev` | search by master level |
| `scname` | search by name |
| `scoption` | search result actions |
| `scresult` | search results |
| `scsibori` |  |
| `searchjo` | search by job |
| `searchle` | search by mission rank |
| `searchma` | search menu |
| `searchna` | search by nation |
| `searchra` | search by race |
| `shop` | shop item list |
| `shopbuy` | buy confirm |
| `shopmain` | shop menu |
| `shopsell` | sell confirm |
| `socialme` | system menu (the one Escape opens) |
| `sortyn` | sort confirm |
| `splmsg2` |  |
| `spoolmsg` |  |
| `statcom` |  |
| `statcom2` | status menu |
| `statpup` | automaton status |
| `storage` | Mog House storage menu |
| `storage2` |  |
| `stringdl` | delivery recipient name entry |
| `subwindo` | sub-target panel under the target window |
| `targetwi` | target window |
| `textcol1` | text color picker |
| `textcol2` | text color picker |
| `textcol3` |  |
| `titlehan` |  |
| `titlewin` |  |
| `trade` | trade window |
| `trddummy` |  |
| `trdskill` | craft skills (status menu) |
| `tskill1` | synthesis confirm |
| `tskill2` |  |
| `ut_menu` | Unity menu |
| `ut_point` | Unity (status menu) |
| `wepsort` |  |
| `worldsel` |  |
