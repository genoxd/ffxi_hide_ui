# ffxi_hide_ui

Hide, move, resize, open, close and answer the windows of Final Fantasy XI's
own interface, from a Windower 4 addon.

The client builds its interface out of 369 named windows. You can

- hide any window, or block it so the game never opens it
- move the windows
- resize windows; their contents do not reflow
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

Three files go in your addon's own `libs/` folder, not Windower's shared
`addons/libs/`. They are prebuilt: copy them from the example addon's folder,
[`examples/hideuidemo/libs/`](examples/hideuidemo/libs).

```
addons/myaddon/
├── myaddon.lua
├── data/                     only if you save a layout, see Save a layout
└── libs/
    ├── hideui.lua            the Lua side, what your addon requires
    ├── _HideUI.dll           the engine it loads
    └── hideui_daemon.dll     the daemon: patches the game's routines
```

```lua
_addon.name = 'myaddon'                 -- every addon sets this; hideui.new() reads it

local hideui = require('libs.hideui')

local ui, why = hideui.new()            -- labeled with your addon's name
if not ui then
    windower.add_to_chat(123, 'myaddon: ' .. why)   -- nil and the reason when the library cannot install
    return
end

ui:on('error', function(e)              -- anything the game refuses lands here
    windower.add_to_chat(123, ('myaddon: %s %s: %s'):format(e.verb, e.name or '', e.reason))
end)
```

`ui` is your handle. Everything you do goes through it, and unloading your
addon undoes all of it: hidden windows come back, blocked ones can open,
moved and resized windows go home. `ui:release()` does the same without
unloading.

The handle carries your addon's name, which other addons see when they ask
who moved a window. To go by another name, pass it to `hideui.new`.

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
example addon from [Try it first](#try-it-first) and turn its event printing
on; every window then prints its name as it opens:

```
//lua load hideuidemo
//hideuidemo events on
```

## Hide and block

```lua
ui:hide('targetwi')        -- the target window is no longer drawn
ui:unhide('targetwi')      -- back

ui:block('equip')          -- the equipment window can no longer open
ui:unblock('equip')        -- it can again
```

A hidden window keeps running but takes no mouse, keyboard or gamepad input,
and it stays hidden through its closes and opens until you unhide it. The one
exception to the input rule is typed text: a hidden text entry, `passinpu`,
still receives characters, though not Enter or Escape. Escape does not close
a hidden window; closing it is your addon's job, with `close`, described
under [Open and close](#open-and-close), which also says which windows take
`cancel` instead.

A blocked window never opens. The open is refused before anything is drawn,
and every addon gets a `blocked` event, described under [Events](#events). If
the window is open when you block it, the library closes it through the
game's own close routine, the one Escape runs, so the window's own cleanup
happens. A prompt that is open when you block it is hidden instead of closed,
because closing it would strand what the game is waiting on; the prompts are
listed under [Prompts](#prompts).

Some windows are part of something the game is in the middle of, and
blocking them has consequences:

| window | if blocked |
|---|---|
| `query` | `block` is refused: the game would crash. Hide it and answer it, see [Prompts](#prompts) |
| `trade` | the trade is canceled |
| `delivery`, the outgoing delivery box | the player cannot move until you call `ui:cancel('delivery')` and the server answers, see [Prompts](#prompts) |
| `post1` and `post2`, the incoming delivery box | the session stays open until `ui:cancel('post1')`, see [Prompts](#prompts) |
| `passinpu`, `link5`, `arealist` | the NPC's script waits until you `answer` or `cancel` it, see [Prompts](#prompts) |
| `prtyjoin`, the party menu's invite screen | the invite waits until you `answer` or `cancel` it |
| `mcr1pall` and `mcr2pall`, the Ctrl and Alt macro bars | the bar no longer shows; for the keys themselves, see [Macro keys](#macro-keys) |

Checked and fine to block: the shop windows, `auc1`, `inspect`, `targetwi`,
`casttime` and the map windows. Windows in neither list have not been checked.

One window takes a second argument. Job abilities, pet commands, weapon
skills and job traits are all the same window, `ability`, showing a
different list, so `block('ability')` would take all four away. Name the
list to block just one: `job_abilities`, `pet_commands`, `weapon_skills` or
`job_traits`, or its number, 1 to 4 in that order, as a string.

```lua
ui:block('ability', 'pet_commands')     -- only the pet command list; the other lists still open
ui:unblock('ability', 'pet_commands')
```

If that list is on screen when you block it, it closes. `unblock('ability')`
with no list drops only a whole-window block; a list is unblocked by naming
it again.

## Move

```lua
ui:move('targetwi', 1500, 500)   -- top-left corner to 1500,500
ui:reset('targetwi')             -- back where the game puts it, position and size
ui:reset('targetwi', 'position') -- one aspect only; 'size' is the other
ui:reset_all()                   -- every window your addon was the last to move or resize
```

Positions are in UI pixels, the units the game lays its interface out in,
with 0,0 at the top left. `hideui.status().ui` gives the width and height,
`{w = 1920, h = 1080}` on a 1080p screen.

Every time the game opens the window again, it lands where you put it.

`reset` touches only what your addon placed; a window another addon placed
is refused with that addon's name.

Some windows are docked: the game places them against another window and
puts them back there whenever that window moves; the target window sits on
the party list, for example. Moving a docked window undocks it.

To move a window with everything docked to it, name the group instead. There
are three groups, each named for its anchor, the window the others are
docked to:

| group | anchor | contains |
|---|---|---|
| `target_window` | `targetwi` | the target window and its sub-target panel |
| `party_list` | `partywin` | the party list and the windows docked on it, the `target_window` group among them |
| `chat_log` | `logwindo` | the chat log, the menus that open above it, and the `party_list` group |

```lua
ui:move_group('chat_log', 16, 830)        -- the chat log, the party list and the target window with it
ui:move_group('party_list', 1700, 900)    -- the party list, the target window, the sub-target panel
ui:move_group('target_window', 1700, 980) -- the target window and the sub-target panel
```

`ui:reset_group('chat_log')` undoes a group move.

The chat log and its second window, `logwin2`, and the party list grow
upward from a fixed bottom: their frame's top moves as lines or members come
and go. `move` and `move_group` still take the frame's top-left for them,
where the frame's top is at the moment of the call, and the library works
out the bottom from the current height.

If a group's anchor is closed when you call `move_group`, the move waits and
happens when the game opens it.

## Resize

```lua
ui:resize('partywin', 3)        -- the party list at 3 rows
ui:resize('playermo', 4)        -- the action menu at 4 rows
ui:resize('targetwi', 200, 60)  -- most windows, to a width and height in UI pixels
```

Rows use the game's own layouts, and three windows have them: `partywin` 1-6,
`playermo` 1-10 and the Monstrosity action menu `mp_pmode` 1-8. A width and
height change only the window's outline; the contents stay as they are.

How long a size lasts depends on the window, and `info(name).resize.holds`
names the case, described under [Read a window](#read-a-window). Most windows
take the size back every time they open, until `reset` undoes it. The party
list and the action menu re-size themselves when the roster or the row count
changes, and the size is then the game's until the next open. The item
descriptions `iteminfo` and `itemxinf` size themselves on every draw, so a
size on them does not last.


## Open and close

```lua
ui:open('equip')
ui:close('equip')
```

`open` works only in the context the game itself opens that window from, so
the equipment window opened outside the main menu closes again at once, and
you see `opened` then `closed`. The prompts listed under [Prompts](#prompts)
can only be opened by the game, and `close` refuses them; use `cancel`.

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

Its box is 88 by 42 UI pixels, and `ui:info('compass').rect` tells you where
it is right now. Left alone, the game keeps the box just above the chat log,
moving it whenever the log grows or shrinks. Once you move it, it stays
where you put it until you reset it.

The game shows and hides the compass by itself, so there is nothing to block
or close: `block` and `close` do the same as `hide`, and `unblock` and `open`
the same as `unhide`. `resize` is accepted and does nothing, because the
compass has no rows and no frame to size, and `info('compass')` reports no
cursor and no elements.

The game hides the compass for a cutscene and shows it again when the
cutscene ends, and closing the map shows it too; the `closed` and `opened`
events follow that.

The clock inside it is switched with the game's own `/clock on` and `/clock
off`; the library leaves it alone.

## Macro keys

Ctrl+1 through Ctrl+0 and Alt+1 through Alt+0 run the game's macros, and
the macro bar is only a display of them: blocking the bar windows leaves
the keys working. To take those keys for your own Windower binds, block the
macros themselves:

```lua
ui:block_macros()     -- Ctrl+number and Alt+number no longer run macros
ui:unblock_macros()
ui:macros()           -- { blocked = true, blocked_by = { 'myaddon' }, mine = true }
```

While the block is on, pressing Ctrl or Alt shows no bar and Ctrl+number and
Alt+number do nothing; a bar that is showing when you call `block_macros`
closes. The block stays on until you call `unblock_macros` or your addon
unloads.

## Events

```lua
ui:on('opened', 'equip', function(e) print('equip opened') end)   -- one window
ui:on('closed', function(e) print(e.name .. ' closed') end)      -- every window
ui:on('blocked', 'ability', 'pet_commands', function(e) end)     -- one list of the ability window

local fn = ui:on('opened', 'equip', function(e) end)
ui:off('opened', 'equip', fn)                                    -- remove one
ui:off('opened')                                                 -- remove every opened handler
```

Every event carries `e.name` except `pending`, `resync` and an `error` about
a call that names no window. The `opened`, `closed`, `covered`, `uncovered`
and `blocked` events for `ability` also carry `e.category`, the game's number
for the list, and `e.category_name`
for the four lists named under [Hide and block](#hide-and-block). Naming the
list as a third argument to `on` or `off`, as in the example, keeps the
handler to that list.

| event | when |
|---|---|
| `opened` | the window is open now: the game opened it, or re-opened it while it was already open, so never count opens against closes |
| `closed` | the window is gone |
| `covered` | another window went over it; it is still open underneath |
| `uncovered` | it came back out |
| `blocked` | the game tried to open a window some addon blocked; `e.mine` is true when the block is yours, and `e.by` lists every addon holding one |
| `error` | the game refused something you asked for, or one of your own callbacks raised. `e.verb` names the call, or is `'callback'`; `e.name` names the window and `e.reason` says why |
| `cursor` | the game's cursor in an open window moved to row `e.row`; for `query` and `arealist`, `e.row` is the index into the list `options()` returns, see [Prompts](#prompts) |
| `pending` | a party invite or a delivery-box session started or ended, see [Prompts](#prompts); `e.what` is `'invite'` or `'post'`, `e.pending` true or false |
| `resync` | the library holds events for you between frames, the last 4096 of them. If your addon falls so far behind that events it has not taken are overwritten, it gets this one event in their place, with `e.dropped` set to how many were lost; re-read `opened()`, `info()` and `pending()` to catch up |

Every event table carries `e.event`, so one handler can serve several
events.

Callbacks run on the frame after the event, so a window may already be gone
by the time `opened` reaches you; check `ui:info(name).open` before acting
on it. A window already open when your addon loads sends no `opened`, so
check what is open on load.

A callback that raises never stops the others. Its first raise comes back
to you as an `error` event with `e.verb == 'callback'`; later raises are
only counted, and `hideui.debug(true)` shows the count, see
[Status and debugging](#status-and-debugging).

## Read a window

```
local p = ui:info('targetwi')     -- nil and a reason for a name that is no window
p.open                -- true/false
p.hidden, p.blocked   -- by any addon
p.rect.x, p.rect.y, p.rect.w, p.rect.h   -- while open
p.cursor              -- the row the game's cursor is on, 1-based, while open; for query and arealist, the index into their list
p.top                 -- query and arealist: the index of the first row on screen; query shows three rows, arealist five
p.focused             -- has the keyboard
p.category            -- ability only, while open: 1 job abilities, 2 pet commands, 3 weapon skills, 4 job traits; p.category_name says which in words
p.blocked_categories  -- ability only: the lists any addon has blocked
p.mine                -- {hidden, blocked, blocked_categories}: what your own addon holds on it
p.elements            -- while open: a list of {type = 'item', x = 44, y = 836, w = 132, h = 16, text = 'Yes'};
                      --   type is 'frame', 'item', 'cursor' or 'other'; the other fields only where the part has them
p.hidden_by, p.blocked_by   -- names of the addons holding a hide or a block on it
p.blockable           -- whether block is allowed on it
p.resize              -- {holds, min_rows, max_rows}: how long a size lasts, and the row range if it has one
                      --   holds is 'reopen': the size is put back every time the window opens;
                      --   'trigger': the window re-sizes itself on a change, and the size is then the game's until the next open;
                      --   'frame': the game sizes the frame every frame, so a size does not last;
                      --   'none': nothing to size
p.memory              -- the remembered position and size, as remembered() lists them
p.covered             -- open but under another window
p.docked              -- the game keeps it against another window
p.detail              -- internals; present only while hideui.debug(true) is on, see Status and debugging

ui:opened()           -- { 'logwindo', 'partywin', ... }: every window open now
ui:focused()          -- the name of the window with the keyboard, or false
ui:list()             -- a table keyed by window name, each with its open, hidden, blocked, moved and resized state
ui:groups()           -- { chat_log = {anchor, anchor_open, origin, members, waiting}, ... }
                      --   anchor: the group's anchor window name; anchor_open: whether it is open; origin: the anchor's current top-left;
                      --   members: the window names in the group; waiting: a group move waiting for the anchor to open, with x, y, owner and mine
ui:remembered()       -- { targetwi = {position = {x, y, owner, mine}, size = {rows | w, h, owner, mine}}, ... }
ui:rects()            -- { logwindo = {x = 16, y = 898, w = 1774, h = 166}, ... }: every open window's frame
```

Three calls look alike: `list()` flags each window as moved or resized by
any addon, `remembered()` gives the positions and sizes the library has put
on windows and who put them there, and `layout()`, under
[Save a layout](#save-a-layout), is what your own addon asked for.

## Prompts

Some windows are the game asking the player something: an NPC's choice list,
a text entry, a party invite. If you hide one to draw your own, you also have
to answer it, or the player is stuck.

```lua
local q = ui:options(name)    -- what the prompt is offering; nil once it is gone
ui:answer(q, value)           -- the player's choice
ui:cancel(q)                  -- the player's cancel
```

`q` carries the prompt's name and an `id` naming this particular open of it,
so a reply meant for a prompt the game has since replaced is refused instead
of landing on the next one. `ui:answer(name, value)` and `ui:cancel(name)`
also work when you don't care which open it is. To name one without the
table, pass its id as the last argument: `ui:answer(name, value, id)` and
`ui:cancel(name, id)`.

Every string the library hands you, in a field named `text`, is UTF-8, the
form Windower's text objects draw, and the game's own bytes sit beside it in
the same table as `raw`. `raw` is the form the game's chat wants, so pass
`raw`, not `text`, to `windower.add_to_chat`. The title and each option of
`query` also carry `segments`: the same text as a
list of runs, each `{text, raw, color}`, where `color` is `'default'`,
`'green'`, or `'color<n>'` for a color the library has no name for. They
also carry `undecoded`, the number of characters the library could not
decode; each one shows as `?` in `text`.

### The NPC choice list

Every list of choices an NPC's dialogue offers is the window `query`: a
yes or no, the home point menus, a treasure chest's items. The game's own
confirmations outside dialogue are a different window, `rem4line`. Hide
`query`, draw the options your own way, and answer with the one the player
picks:

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
which does nothing with them while a hidden list is up.

`ui:options('query')` returns:

```lua
{
    name = 'query',
    id = 12,                        -- this particular open of the prompt
    title = { text = 'Teleport where?', raw = '...' },   -- segments and undecoded left out
    cancellable = true,             -- whether backing out is allowed here
    options = {                     -- in the game's order; raw, segments and undecoded left out of each
        { text = 'Nowhere.',            value = 1 },
        { text = 'Home Point #1 (E).',  value = 2 },
        { text = 'Home Point #4.',      value = 5 },   -- values skip where the game left options out
    },
}
```

Answer with an option's `value`, never its position: "Yes" is not always
1.

### The other prompts

| window | `ui:options(name)` | `ui:answer(name, ...)` | `ui:cancel(name)` |
|---|---|---|---|
| `passinpu` text entry | `{max_length = 16}` | the text, up to `max_length` bytes; longer is refused | no text |
| `prtyjoin` party invite | `{inviter = 'Somebody', alliance = false}` | `true` accept, `false` decline | decline |
| `link5` linkshell list | `{slots = {{slot = 1, name = 'MyShell'}, ...}}` | a `slot` from the list | close, no choice |
| `arealist` area list | `{name, id, pending, mode, level, rows = {{text = 'Bastok Mines', id = 234, kind = 'zone'}, ...}}`, every row the game shows, in order | a zone `id` from the rows | closes the list with no zone |
| `delivery` outgoing box | refused | refused | end the session |
| `post1`, `post2` incoming box and its second screen | refused | refused | end the session |

The text for `passinpu` is bytes as the game's text entry takes them: plain
ASCII goes in as it is; anything else your addon converts with
`windower.to_shift_jis` before calling `answer`, and the library passes the
bytes through unchanged.

`ui:options('arealist')` adds `mode` and `level` of its own. `mode` says who
opened the list: 1 or 2 means an NPC opened it and is waiting, so `answer`
and `cancel` work; any other value is a menu the player opened, which the
library can only read. `level` is 0 at the top of the list; inside a region
it equals that region row's `id`.

Each row is `{text, id, kind}`, and `kind` is one of:

- `zone`: an area; `id` is its number in Windower's `res.zones`
- `region`: a heading the player can open; `id` is the region's number, negated
- `current_area`, `current_region` and `all`: the list's rows for the current area, the current region and all areas
- `other`: any row that is none of the above

A row may also carry `label`, the text beside it, with its bytes in
`label_raw`, or `count`, a region's number of zones.

A blocked list has no rows, and `answer` then accepts any zone id, 0 to 511.

With a box open, `cancel` does what the box's own close control does, and
the windows close when the server answers, a moment later.

A party invite shows no window until the player opens the party menu, and
a delivery-box session can be up with its windows blocked. The `pending`
event tells you when either appears, and `ui:pending()` shows what is
waiting; its `invite` and `post` tables go straight into `answer` and
`cancel`:

```lua
local w = ui:pending()
if w.invite then ui:answer(w.invite, true) end     -- w.invite.inviter, w.invite.alliance
if w.post then ui:cancel(w.post) end               -- w.post.box is 'delivery' or 'post1'
```

## Save a layout

To bring positions and sizes back after a reload, ask for them, save them,
and apply them on load:

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

`ui:layout()` is a plain table of what your addon asked for, which the config
library can save as it is. It is yours alone: another addon moving the same
window later does not change it.

`ui:apply()` refuses a layout saved at a different UI size, which the table
records. When some entries cannot be placed, it places the rest and returns
`nil`, a reason and the list of those entries; `layout()` keeps them, so they
are saved and tried again next time, until the window is reset or a later
`apply` places them.


The config library writes `data/settings.xml`, and the game client cannot
create folders: a write into a folder that is missing freezes the client. Ship
an empty `data/` with your addon.

## More than one addon

Several addons can use the library at once. The engine in charge is the
`_HideUI.dll` of whichever addon loaded first; every later addon's copy talks
to it, and once all of them have unloaded the next addon to load starts
afresh with its own. Hides and blocks add up: a window hidden or blocked by
two addons stays that way until both let go,
and the same goes for the macro keys. Moves do not add up: the library
keeps one position per window, so a window moved by two addons sits where
the later move put it, and when that addon unloads the window goes back to
where the game puts it, not to the earlier addon's spot.

## Status and debugging

```lua
hideui.version()      -- 'hideui 0.10.0'; goes in a bug report
hideui.debug(true)    -- print your addon's library failures to chat; off, it prints nothing
hideui.status()       -- the fields below
ui:status()           -- the same, with .dropped for this handle only
```

`hideui.status()` describes the engine for the whole game:

| field | meaning |
|---|---|
| `ok` | whether the engine installed |
| `ui` | the game's UI size, `{w, h}` |
| `dropped` | events lost across every addon's handles |
| `macros_blocked` | whether any addon blocks the macro keys |
| `hidden`, `blocked`, `moved`, `resized` | the window names any addon holds that way |

`hideui.debug(true)` prints why `new` failed, calls the game refused,
callbacks that raised and events that were lost. While it is on, `status()`,
`info()`, `list()` and `pending()` also carry the engine's internals under
`detail`, the count of callback raises among them.

## When it cannot work

`require('libs.hideui')` raises when `_HideUI.dll` is missing or will not
load; wrap it in `pcall` if your addon should survive that. `hideui.new`
returns `nil` and a reason when the library cannot install. Print the
reason; the install example above returns at that point. The reasons:

- `hideui_daemon.dll` is missing beside `_HideUI.dll`
- the daemon another addon loaded first is older than yours
- the engine in charge is older than your addon's copy and lacks a call
  yours needs, see [Updating](#updating-the-library-in-your-addon)
- a game patch changed the code the library relies on

## Try it first

[`examples/hideuidemo`](examples/hideuidemo) turns the library's calls into
commands, so you can try things before writing code. Copy the folder,
`data/` included, into Windower's `addons/`. Some of its commands, with
`//hideuidemo help` listing them all:

```
//lua load hideuidemo
//hideuidemo events on          print every event as it happens
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
//hideuidemo layout          save the demo's layout to data/settings.xml
//hideuidemo apply           put a saved layout back
//hideuidemo status
//hideuidemo debug on        print the library's own failures
//hideuidemo help
```

`//hideuidemo list` prints every window name, and `//hideuidemo list item`
the names containing "item", each with its state.

## Updating the library in your addon

When your addon's copy of the engine is older than the engine in charge, it
works as it is. When yours is newer and needs a call the older engine does
not have, `new` returns `nil` and names the engine in charge by its path, so
you know which addon to update. A field the older engine does not know is
missing from what you read.

To replace `_HideUI.dll`, unload every addon that uses the library, wait a
few seconds, then copy the new file in; never write over it while it is
loaded. Replace `hideui_daemon.dll` with the game closed.

## Under the hood

The library finds the game's list of its windows and the routines it needs
in the running game, and hooks nine of them: the ones that open, show and
close a window, run the UI each frame, decide whether the mouse is in a
menu, pass a key to a window, open one of the ability window's lists, draw
the compass and let macro keys run. It makes every change from inside the
game's own thread on the game's next frame, which is why a call only queues
the change and returns at once.

The patches on the game's routines belong to `hideui_daemon.dll` rather
than to the engine, so that the engine can be unloaded and replaced while
the game runs. The daemon stays until the game closes, which is also why an
older daemon another addon loaded cannot be swapped for yours.

Building it: [`engine/README.md`](engine/README.md) and
[`daemon/README.md`](daemon/README.md).

## All window names

Every name the library accepts, with a description where one is known,
taken from the window's own captions or the menu it opens from. A blank
means it has not been identified yet; the name is still valid.

| name | window |
|---|---|
| `ability` | job abilities, weapon skills, pet commands and job traits: one window, `info` says which |
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
| `bluesibo` |  |
| `bluinven` | blue magic set list |
| `blusortw` | blue magic sort menu |
| `btlskill` | combat skills (status menu) |
| `buff` | status effect icons |
| `camparea` |  |
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

## License

0BSD. See [LICENSE](LICENSE).
