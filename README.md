# ffxi_hide_ui

Hide, move, resize, open, close and answer the windows of Final Fantasy XI's
own interface, from a Windower 4 addon.

The client builds its interface out of 366 windows. You can

- hide any window, or block most of them so the game never opens them
- move the windows
- resize windows: by rows where the game has row layouts, otherwise only the
  outline
- open and close windows
- subscribe to events when the game opens, closes, covers or uncovers a
  window, or tries to open one that is blocked
- read any window: whether it is open or has focus, where it is, which row the
  cursor is on, what its elements say
- read and answer the game's prompts in place of their windows: NPC dialogue
  and choice lists, yes/no confirmations, number and text entry, trades
- read and change every setting of the game's Config menu, with its pages
  closed: volumes, chat filters, text colors, keys and the rest

Three things that are not windows are covered as well:

- the compass, which you can hide and move like a window
- the arrows over your target, which you can hide without hiding the target
  window
- the macro keys, which you can block so that Ctrl+number and Alt+number are
  free for your own binds

Nameplates, damage numbers, the mouse cursor and the loading screen's graphics
are not windows, and the library does not cover them.

This library draws nothing (shameless plug for
[ffxi_world_draw](https://github.com/genoxd/ffxi_world_draw)).

## Install

Three files go in your addon's own `libs/` folder, not Windower's shared
`addons/libs/`. They are prebuilt: copy them from the example addon's folder,
[`examples/hideuidemo/libs/`](examples/hideuidemo/libs).

```
addons/myaddon/
├── myaddon.lua
├── data/                     create it yourself if you save a layout; see Save a layout
└── libs/
    ├── hideui.lua            the Lua side, what your addon requires
    ├── _HideUI.dll           the engine it loads
    └── hideui_daemon.dll     the daemon: patches the game's routines
```

```lua
_addon.name = 'myaddon'                 -- every addon sets this; hideui.new() reads it

local hideui = require('libs.hideui')

local ui, why = hideui.new()            -- nil and the reason when it cannot install
if not ui then
    windower.add_to_chat(123, 'myaddon: ' .. why)
    return
end

ui:on('error', function(e)              -- anything the game refuses lands here; see Events
    windower.add_to_chat(123, ('myaddon: %s %s: %s'):format(e.verb, e.name or '-', e.reason))
end)
```

`ui` is your handle. Everything you do goes through it, and unloading your
addon undoes all of it: hidden windows come back, blocked ones can open, moved
and resized windows go back where the game puts them. `ui:release()` does the
same without unloading.

The handle carries your addon's name, which other addons see when they ask who
moved a window. To go by another name, pass it to `hideui.new`.

## Return values and errors

Calls that change something return `true`, or `nil` and a reason. `true` means
the change is queued for the game's next frame; if the game refuses it then,
the refusal arrives as an `error` event, with `e.verb` naming the call.

Calls that read something return what they read, or `nil` and a reason.

Misuse raises an error at your line instead of returning:

- a wrong argument type
- `ui.hide` written for `ui:hide`
- an unknown event or window name in `on`
- an [`answer`](#prompts) or [`set_setting`](#settings) value of the wrong
  kind, such as a string where the window or the setting takes `true` or
  `false`
- a count or amount that is not a whole number

## Window names

Every window has a short name of the game's own, up to eight characters, such
as:

- `logwindo`, the chat log
- `partywin`, the party list
- `targetwi`, the target window
- `equip`, the equipment window
- `inventor`, the inventory
- `iteminfo`, an item's description
- `query`, every NPC choice list

`compass` and `targetarrow` are the library's own names, for two things that
are not windows.

To find a name, load [the example addon](#the-example-addon) and run
`//hideuidemo events on`: every window then prints its name as it opens.
`//hideuidemo list item` lists the names containing "item". Every name is
listed under [All window names](#all-window-names).

Three windows have a second name, which the game uses for another layout or
language of the same window:

- `mcr1edlo` and `mcr2edlo` are the macro palettes `mcr1edit` and `mcr2edit`
  in their other layout. A configuration setting picks that layout, and an
  interface narrower than 1024 always uses it.
- `jobcsel` is the job list `jobcselu` on a Japanese client.

Every verb takes either name. Every event, list and read reports the window
under its first name.

## The example addon

[`examples/hideuidemo`](examples/hideuidemo) is a working addon that turns the
library's calls into commands, so you can try things before writing code. Copy
the folder, with its `data/`, into Windower's `addons/`; its `libs/` already
holds the three library files. `//hideuidemo help` lists every command; these
are a start:

```
//lua load hideuidemo
//hideuidemo events on                print every event as it happens
//hideuidemo hide logwindo
//hideuidemo unhide logwindo
//hideuidemo block equip
//hideuidemo move targetwi 1500 500
//hideuidemo group chat_log 16 830    move the chat log with what is docked to it
//hideuidemo resize partywin 3
//hideuidemo reset targetwi
//hideuidemo reset targetwi position
//hideuidemo resetgroup chat_log      undo that group move
//hideuidemo rects                    every open window's frame
//hideuidemo info equip               what the library reads from one window
//hideuidemo opened                   the windows open now
//hideuidemo list                     every window name
//hideuidemo list item                the names containing "item", with their state
//hideuidemo options query            with an NPC's list open
//hideuidemo answer query 2
//hideuidemo cancel query
//hideuidemo answer comyn yes         with a yes/no up
//hideuidemo offer trade 5 1          with a trade open: inventory index 5, count 1
//hideuidemo pending                  questions waiting with no window
//hideuidemo settings                 every Config setting, with its value and range
//hideuidemo settings conf3win        the settings of one Config page
//hideuidemo settings color           the settings whose names contain "color"
//hideuidemo set music_volume 40
//hideuidemo set weather_effects off
//hideuidemo set shadows high
//hideuidemo set color_say 255 255 255
//hideuidemo layout                   save the demo's layout to data/settings.xml
//hideuidemo apply                    put a saved layout back
//hideuidemo status                   the engine's state
//hideuidemo debug on                 print the library's own failures
```

## Hide and block

```lua
ui:hide('targetwi')        -- the target window is no longer drawn
ui:unhide('targetwi')      -- back

ui:block('inspect')        -- the inspect window can no longer open
ui:unblock('inspect')      -- it can again
```

A hidden window is not painted, but the game runs it exactly as it would if
shown. It opens, closes and is cleaned up on the game's own schedule, keeps
its timers, and does its own work each frame, such as closing itself when
what it shows goes away. So a window the game closes while it is hidden
really goes away: it leaves `opened()`, and its `closed` event comes at the
moment it would for a shown window. The window stays hidden through its
closes and opens until you unhide it, and unhiding shows it as it stands at
that moment.

Hiding saves the game no work: a hidden window costs as much each frame as a
shown one.

A hidden window also keeps its place among the game's windows. It gets the
keyboard focus when it would if shown, and loses it when the game gives the
focus to another window, so in a flow of several windows the player's keys
follow the game's own order. Clicks pass through it.

What happens to keys depends on the window:

- A prompt, one of the windows where the game asks the player something,
  takes no keyboard or gamepad input while it is hidden and asking, and
  Escape does not close it. Your addon draws the question its own way, reads
  the keys through Windower and replies with `answer` or `cancel`, described
  under [Prompts](#prompts).
- A hidden prompt that is not asking anything takes the player's keys as it
  would if shown.
- Any other hidden window still takes the keys the game sends it. The party
  list, which `<stpt>` and `<stal>` drive through its keys, keeps working
  hidden.

A prompt is asking unless `options` says nothing is pending in it or that it
is not open. The area list asks only while an NPC waits on it, with `mode` 1
or 2. `delivery`, `post1` and `post2` ask whenever they are up.

The game can leave a prompt up, or give it the focus again, with nothing in it
to answer. Once the player sends a delivery, for example, the game focuses
`stringdl` again, empty, and the player's Escape is what leaves it. So expect
the player's keys to reach a hidden prompt while your own interface is up.

Keys that reach a hidden window that is not asking are handled exactly as
they would be if it were shown, down to the game ignoring them for the brief
moment after the window opens.

Hiding a window while the player is typing in it closes its text field, and
the typed text is lost. The game keeps typed text in one entry line of its
own, not in the window, so the library ends the typing as the window itself
would. The same happens when such a window opens hidden with its field
already active, and whenever its field starts while the window is hidden; the
player's keys can start one in a hidden window that is not asking anything.
Unhiding `passinpu`, `comment` or `msgline` opens the field again, as the game
does when it opens them.

A blocked window never opens. The open is refused before anything is drawn,
and every addon gets a `blocked` event, described under [Events](#events).

If the window is open when you block it, the library closes it as Escape
would, through the game's own close routine, so the window's own cleanup
happens. A prompt that is open when you block it is hidden instead of closed,
because closing it would strand what the game is waiting on; `unblock` shows
it again.

Some windows cannot be blocked or closed, whichever addon asks. `block` and
`close` refuse them and say why; hide them instead:

| window | why |
|---|---|
| `query`, the NPC choice list | the game reads the list as it opens, so a refused open crashes the client |
| `gmtell`, a GM's question | the same: a refused open crashes the client |
| `comyn`, the yes/no confirmation | some of its questions read the window right after asking, so a refused open crashes the client |
| `resyn`, `resynus`, `resynde`, `resynfr`, the raise prompt | the player could never accept a raise |
| `dead`, the KO window | if it is blocked or closed during a raise offer, the game declines the raise and can send the player to their home point at once |
| `mp_dead`, the Monstrosity KO window | if it is blocked, the game at once repeats the choice made at the previous KO |
| `rem4line`, `rem4li2`, the NPC dialogue | nothing moves the dialogue on, and the NPC's script waits forever |
| `stringdl`, the delivery box's name entry | the delivery box gives up and closes |
| `gift`, your side of a trade | if blocked, it does not open with the next trade, but the trade still moves its cursor through `gift`'s window, so the player's keys on the trade crash the client. A close is refused only while `trade` is open, for the same reason |

Other windows can be blocked, with consequences the next time the game tries
to open them:

| window | if blocked |
|---|---|
| `trade` | the trade is canceled |
| `handover`, the NPC trade | the NPC trade does not start |
| `targetwi` | the target window and the arrows over your target never appear; see [The target arrows](#the-target-arrows) |
| `delivery`, the outgoing delivery box | the player cannot move until you call `ui:cancel('delivery')` and the server answers. See [Everything else](#everything-else) |
| `post1` and `post2`, the incoming delivery box | the delivery-box session stays open until `ui:cancel('post1')` |
| `passinpu`, `link5`, `arealist` | the NPC's script waits until you `answer` or `cancel` it by name. Watch for its `blocked` event; it does not appear in [`pending()`](#questions-with-no-window) |
| `prtyjoin`, the party menu's invite screen | the invite waits until you `answer` or `cancel` it |
| `moneyctr`, `itemctrl`, `sortyn`, `evitem01`, `bankmenu`, `maplist`, `ok`, `mogdoor`, `msgline` | the question waits with no window, to be answered or canceled through the library. See [Questions with no window](#questions-with-no-window) |
| `comment`, `scname`, `link11` | nothing waits: the message, the search or the linkshell does not happen |
| `merityn`, `shopbuy`, `shopsell` | the game's question is never asked, and what asked it does not happen |
| `mcr1pall` and `mcr2pall`, the Ctrl and Alt macro bars | the bar no longer shows; for the keys themselves, see [Macro keys](#macro-keys) |

These have been checked and are fine to block:

- `auc1`, `inspect` and `casttime`
- the shop windows, except the confirmations `shopbuy` and `shopsell`
- the map windows, except the list `maplist`

Windows not named in this section have not been checked. Blocking one may
strand what the game is doing, so hide it instead until you know.

One window takes a second argument. Job abilities, pet commands, weapon
skills, job traits and the rest are all one window, `ability`, showing a
different list, so `block('ability')` would take them all away. To block just
one list, give its name, or its number as a string; the lists are in the
table under [Lists](#lists).

```lua
ui:block('ability', 'pet_commands')     -- only the pet command list; the other lists still open
ui:unblock('ability', 'pet_commands')
```

If that list is on screen when you block it, it closes. `unblock('ability')`
with no list drops only a whole-window block; to unblock a list, name it
again.

## Move

```lua
ui:move('targetwi', 1500, 500)   -- top-left corner to 1500,500
ui:reset('targetwi')             -- back where the game puts it, position and size
ui:reset('targetwi', 'position') -- one aspect only; 'size' is the other
ui:reset_all()                   -- every window your addon was the last to move or resize
```

Positions are in UI pixels, the units the game lays its interface out in, with
0,0 at the top left. `hideui.status().ui` gives the width and height,
`{w = 1920, h = 1080}` when the game's UI resolution is 1920 by 1080.

Every time the game opens the window again, it lands where you put it. Some
windows also change layout while open: the party list after a zone change or
when the party changes size, and the action menu when its commands change. The
game moves such a window back to its default spot as it changes, and the
library puts it back at yours in the same frame.

`reset` touches only what your addon placed; a window another addon placed is
refused with that addon's name.

Some windows are docked: the game places them against another window and puts
them back there whenever that window moves; the target window sits on the
party list, for example. Moving a docked window undocks it.

To move a window with everything docked to it, name the group instead. There
are three groups, each named for its anchor, the window the others are docked
to:

| group | anchor | contains |
|---|---|---|
| `target_window` | `targetwi` | the target window and its sub-target panel |
| `party_list` | `partywin` | the party list and the windows docked on it, the `target_window` group among them |
| `chat_log` | `logwindo` | the chat log, the menus that open above it, and the `party_list` group |

```lua
ui:move_group('chat_log', 16, 830)        -- the chat log, the party list and the target window with it
ui:move_group('party_list', 1700, 900)    -- the party list, the target window, the sub-target panel
ui:move_group('target_window', 1700, 980) -- the target window and the sub-target panel
ui:reset_group('chat_log')                -- undo a group move
```

If a group's anchor is closed when you call `move_group`, the move waits and
happens when the game opens it.

Three windows grow upward from a fixed bottom: the chat log `logwindo`, the
second chat log `logwin2` and the party list `partywin`. You still pass the
top-left corner to `move` and `move_group`. The library keeps the bottom edge
at your y plus the window's height when you move it, and the window grows and
shrinks upward from it.

## Resize

```lua
ui:resize('partywin', 3)        -- the party list at 3 rows
ui:resize('playermo', 4)        -- the action menu at 4 rows
ui:resize('targetwi', 200, 60)  -- most windows, to a width and height in UI pixels
```

Rows use the game's own layouts, and five windows have them:

- `partywin`, 1 to 6
- `playermo`, 1 to 10
- `mp_pmode`, the Monstrosity action menu, 1 to 8
- `iteminfo` and `itemxinf`, the item descriptions, 3 to 12

A width and height change only the window's outline; the contents stay as
they are.

How long a size lasts depends on the window, and `info(name).resize.holds`
says which case it is:

- `'reopen'`, most windows: the size is put back every time the window
  opens, until `reset` undoes it.
- `'trigger'`, the party list and the action menu: the window resizes itself
  when the roster or its commands change, and the size is then the game's
  until the next open. Unlike a position, which the library puts back at
  once, a size there follows the game.
- `'frame'`, the item descriptions `iteminfo` and `itemxinf`: the game sizes
  them on every draw, so a size does not last.
- `'none'`: the window has nothing to size.

## Open and close

```lua
ui:open('logwindo')                      -- the chat log, through the game's own open
ui:open('ability', 'pet_commands')       -- one list of the ability window, alone
ui:open('jbpcat', 'thf')                 -- a window that takes an argument: thief's Job Points
ui:open('iteminfo', {bag = 'inventory', slot = 5})
ui:open_set('equipment')                 -- the equipment window with the windows it works with
ui:open_set('magic', 'white_magic')      -- the list with its sort window and its detail pane
ui:open_set('synthesis', 12)             -- synthesis with the crystal at inventory index 12
ui:close('logwindo')
```

`open` opens exactly the window you name, through the game's own open
routine, and nothing with it. It does so whether or not the game ever opens
that window that way, as long as the window works on its own. A window that
does not is refused, and the reason names what to use instead; those windows
are listed [below](#windows-open-refuses).

The game can still close a window again at once by its own rules: the main
menu closes itself during a cutscene, for one.

Only the game can open a prompt. The prompts are listed under
[Prompts](#prompts).

`open` calls the game's open routine as the game's own openers call it. For
all but four windows, that shows the window at once, over whatever else is
up. The four are `gaugewin`, `targetwi`, `playermo` and `mp_pmode`, which the
game opens differently: while a window they may not show over is up, they
open underneath it and come out once it goes. An `open` of one of them does
the same. Its `opened` event comes at once, `info(name).covered` is true
while it waits, and `uncovered` follows when it comes out.

### Lists

The ability window and the magic window each show one list at a time, and
`open` on either needs the list, by its name or its number:

```lua
ui:open('ability', 'weapon_skills')
ui:open('magic', 4)                      -- ninjutsu
```

| ability list | number | | magic list | number |
|---|---|---|---|---|
| `job_abilities` | 1 | | `white_magic` | 1 |
| `pet_commands` | 2 | | `black_magic` | 2 |
| `weapon_skills` | 3 | | `summoning` | 3 |
| `job_traits` | 4 | | `ninjutsu` | 4 |
| `blood_pact_rage` | 6 | | `songs` | 5 |
| `phantom_roll` | 8 | | `blue_magic` | 6 |
| `quick_draw` | 9 | | `geomancy` | 7 |
| `blood_pact_ward` | 10 | | `trust` | 8 |
| `sambas` | 11 | | | |
| `waltzes` | 12 | | | |
| `steps` | 13 | | | |
| `flourishes_i` | 14 | | | |
| `stratagems` | 15 | | | |
| `jigs` | 16 | | | |
| `flourishes_ii` | 17 | | | |
| `ready` | 18 | | | |
| `flourishes_iii` | 19 | | | |
| `monstrosity` | 20 | | | |
| `rune_enchantment` | 21 | | | |
| `ward` | 22 | | | |
| `effusion` | 23 | | | |

The game checks no list number, and one past the end of a window's lists
writes past that window's own memory, so `open` refuses any name or number
not in the table.

The same names go everywhere a list is named:

- `block` and `unblock`, which also take any number from 1 to 31
- `on` and `off`, which accept any string, so a misspelled name never matches
- the `category_name` the events carry

`open` opens the list alone. The ability window opens without its sort window
`abisortw` or its detail pane `subwindo`, and the magic window without its
sort window. The game's magic list opens `subwindo` every time it opens; the
library stops that open, so no `subwindo` appears and no event about it is
posted.

A list the game will not show closes again at once: a pet's list
(`pet_commands`, `blood_pact_rage`, `blood_pact_ward`, `ready`) with no pet
out, and `monstrosity` outside Monstrosity. The `error` event then says the
game refused it.

### Windows that take an argument

Some windows show whatever their opener tells them to show: which storage
list, which job, which item. The game writes that into the window just before
it opens it, so a window opened by name alone shows what the last opener left
there, or nothing. A few of them crash the client that way, and a few send
the server a request made of nothing.

For each of these windows, `open` runs the game's own route to it instead:
the one its menu row, command or list runs. What that route needs goes in as
the second argument. The argument is a name, a number (or the same number as
a string), or a table of named fields. Only the timers take a fraction:

```lua
ui:open('mogpost', 'nomad')
ui:open('fp_cat2', 3)
ui:open('casttime', {seconds = 10, style = 'countdown'})
```

When you name no argument, a window with a default opens with it. One without
a default is refused, and the reason gives an example.

Where the game's route opens another window along with the one you named,
that window is held off, as `subwindo` is for the lists above: `open` still
opens only what you name. The sets in the [next section](#sets) open the
windows together.

| window | call | what the argument means | default |
|---|---|---|---|
| `storage` | `open('storage')` | none: the Mog House's storage list (Mog Safe, Storage, Mog Locker, Satchel, Sack, Case, Wardrobes), in the Mog House only | |
| `storage2` | `open('storage2')` | none: the Nomad Moogle's storage list, the same bags without Storage, only where a Nomad Moogle serves | |
| `mogpost` | `open('mogpost', mode)` | `'mog_house'`: the Mog House's post menu, whose rows run `/mailbox` and `/sendpost`, in the Mog House only; `'nomad'`: the Nomad Moogle's, whose rows ask the server for the delivery box and the send box | `'mog_house'` |
| `friend` | `open('friend')` | none: your friend list. The header strip `titlehan` is held off | |
| `msglist` | `open('msglist')` | none: your PlayOnline mail. Every open asks PlayOnline for the list again. `titlehan` is held off | |
| `blklist` | `open('blklist')` | none: your blacklist | |
| `mount` | `open('mount')` | none: the mount list, as the ability list opens it, with no sort window. Its detail pane `subwindo` is held off | |
| `jobcselu` | `open('jobcselu', which)` | `'main'` or `'support'`: the job list for changing that job. A job picked in it changes your job | required |
| `conf11l` | `open('conf11l', set)` | 1 or 2: one of the two sets of message settings, as the first or second row of the configuration's message menu `conf11m` opens it. The game labels the two sets with images, not text, so they go by number | 1 |
| `jbpcat` | `open('jbpcat', job)` | a job by its three letters, `'war'` to `'run'`, or Windower's job id, 1 to 22: that job's Job Points categories | required |
| `fp_cat2` | `open('fp_cat2', category)` | 1 to 7: the upgrades of one category of `fp_cat`, the Status menu's twelfth list | required |
| `quest00` | `open('quest00', mode)` | 1: the quest log's menu, as the quest log command opens it. Its rows are "Current", "Completed", "Current" (with a count of 30), "Objective List", and a row that reads "Vana", a symbol of the game's own font, then "Bout". 2 or `'Unity Information'`: the rows "Active" and "Inactive" under "Unity chat settings:". 3 or `'Ranking Information'`: the rows "Ranking Results" and "Partial Ranking Results". Mode 1 has no title in the game's text, so it goes by number | 1 |
| `quest01` | `open('quest01', list)` | 1 to 7, the row of `quest00` it opens from: 1 to 5 are the rows of its mode 1, in that order, and 6 and 7 those of its mode 3. Two rows read "Current", so the lists go by number. Refused unless `quest00` is open in that mode, since the game opens them only from there | 1 |
| `miss00` | `open('miss00', log)` | none: your nation's mission log, as the Missions menu's first row opens it; or a log by its name in the game, in any case: `"San d'Oria"`, `'Bastok'`, `'Windurst'`, `'Rise of the Zilart'`, `'Chains of Promathia'`, `'Assault'`, `'Treasures of Aht Urhgan'`, `'Campaign'`, `'Wings of the Goddess'`, `'Seekers of Adoulin'`, `"Rhapsodies of Vana'diel"`, `'The Voracious Resurgence'`, `'A Crystalline Prophecy'`, `"A Moogle Kupo d'Etat"`, `'A Shantotto Ascension'`, or its number in that order, 1 to 15 | your nation's |
| `searchjo`, `searchna`, `searchra`, `searchle`, `scchar` | `open('searchjo', mode)` | `'new'`: a fresh search of your area, after the same reset of the search that the Search row runs first; `'refine'`: added to the search on screen. A pick starts a real search on the server | `'new'` |
| `comgenre` | `open('comgenre', mode)` | `'comment'`: set the category of your own search comment; `'search'`: search other players' comments, from a fresh search; `'refine'`: narrow the search on screen | required |
| `sccompar` | `open('sccompar', {purpose = 'comment', category = 2})` | the sub-categories of comment category 1, 2 or 3, for your own comment (`'comment'`) or to narrow the search on screen (`'refine'`) | required |
| `sccomite`, `sccomls` | `open('sccomite', purpose)` | `'comment'` or `'refine'`: the sub-categories of comment category 4 or 5 | required |
| `iteminfo` | `open('iteminfo', item)` | `{bag = 'inventory', slot = 5}`: the item in that slot. The bag goes by its name or Windower's bag id (`inventory` 0, `safe`, `storage`, `temporary`, `locker`, `satchel`, `sack`, `case`, `wardrobe`, `safe2`, `wardrobe2` to `wardrobe8`, `recycle` 17). The slot is 1 to 80, as `windower.ffxi.get_items` reports it, up to the bag's size now. Or `{item = 16555}`: the plain item with that id, 1 to 65534, as a shop shows it | required |
| `itemxinf` | `open('itemxinf', item)` | as for `iteminfo`: the compare option's panel for the piece you wear, as the inventory's cursor opens it beside the equipment and the inventory. `itemyinf`, which opens with it, is held off | required |
| `casttime` | `open('casttime', length)` | seconds, or `{seconds = 3, style = 'countdown'}`: the bar a spell's casting fills (`'bar'`, the default style), or the seconds left as an NPC's event shows them (`'countdown'`). The timers count in sixtieths of a second, so the length is rounded to the nearest sixtieth. It runs from one sixtieth to 2147483647 of them (just over 35791394 seconds), the most the game's count holds | required |
| `eventtim` | `open('eventtim', seconds)` | seconds, as for `casttime`: the event timer | required |
| `persona` | `open('persona')` | none: the status pane in the equipment screen's layout, alone | |
| `macro` | `open('macro', {palette = 'ctrl', row = 3})` | `'ctrl'` or `'alt'`, and 1 to 10: the editor of that macro, in the macro set your keys use now. If you name the palette that is not up, the game's palette switch runs first, as Left or Right on a palette does, its sound included, and the other palette's window it opens is held off. The copy and paste bar `mcrledit` is held off | required |
| `scanlist` | `open('scanlist')` | none: the Wide Scan list, as the map's Wide Scan row opens it. While the map tracks someone, the row first asks on `comyn` whether to stop, and so does the open. Answer it as any [yes or no](#yes-or-no); a yes stops the tracking and opens the list | |
| `coloresu` | `open('coloresu', region)` | 2 to 9: the status pane of one region of the Colonization map (region 1 has the ranking pane, `colorank`, instead) | required |

Every value outside a window's table is refused at once. The game checks few
of them, and a wrong one shows garbage, saves settings nowhere, or sends the
server a category or job it never would.

The routes also refuse what the game's own tests refuse first. That refusal
comes as an `error` event from the game's thread:

- `jobcselu`, and `mogpost` with `'nomad'`, outside the Mog House and the
  areas where a Nomad Moogle serves. The job change acts through the manager
  of one or the other, and the game makes no other check, not even your
  distance to the moogle.
- `storage` and `mogpost` with `'mog_house'` outside the Mog House, and
  `storage2` where no Nomad Moogle serves: the Mog House's menu and the
  Nomad Moogle's are the only rows that open them, and each exists only
  where its manager does.
- `jbpcat` for a job under level 99, which the Job Points list does not
  offer you.
- `quest01` with a list that `quest00`'s current mode does not offer: lists
  1 to 5 need its mode 1, and lists 6 and 7 its mode 3. The refusal names
  the mode.
- `miss00` with a log whose expansion is not enabled on your account.
- `'refine'`, and `scsibori`, before a search has run. A pick appends to the
  search's criteria, which are not set up before a search, so the pick would
  write far past them.
- `iteminfo` and `itemxinf` for an empty slot or one past the bag's size
  now, which the game's lists never show.
- `iteminfo` and `itemxinf` by id for an item of types 20 to 23 (Assault
  logs, Bonanza Marbles and Pearls, Maze Tabulae). The panel reads their
  extra data with no test, and only the bag's copy has that data, so by id
  alone it crashes the client. Give their bag and slot.
- `msglist` while PlayOnline's mail is busy or not there.
- `coloresu` unless `cnqframe` shows its Colonization map.
- `macro` before the game has made its macro editor, which it makes the
  first time you open the macro menu.

Several of these act on the game as the player's own use would:

- a job picked in `jobcselu` changes your job
- the Nomad Moogle's post rows ask the server for your boxes
- a search picker's pick starts a search
- `fp_cat2` asks the server for its data when it has none

`casttime` and `eventtim` stand for a real cast or event timer. While one is
up, crystal synthesis is refused, and an NPC event that wants its own timer
reads back the time left on yours.

An addon built for an older version of the library that calls `open(name)`
on a window with a default gets the route as well. A window that needs an
argument is refused, whichever version of the library asks, because opened
by name alone it would show what the last opener left in it, or crash the
client.

Where the game has a name for an argument in its text, the argument uses it.
The mission logs are the names the game's mission list shows, and `quest00`'s
modes 2 and 3 are the titles its openers show. The row texts quoted for
`quest00` are the game's own text for those rows.

The argument goes by number in two cases:

- where the game labels its rows with images instead, as for `conf11l`'s
  sets and the text color categories
- where two rows read the same, as for `quest01`'s lists

The other names here name what the game's route does, not a label the game
shows: `'mog_house'` and `'nomad'`, `'main'` and `'support'`, `'new'`,
`'refine'`, `'comment'` and `'search'`, `'bar'` and `'countdown'`, `'ctrl'`
and `'alt'`. The jobs and bags are Windower's names for them, from its
resources.

### Sets

Some screens are several windows that the game will not let stand one at a
time: it closes the others again, or they open broken.

`open_set` opens such a screen through the game's own opener for it: the one
the player's menu row, shortcut, command or item use runs. It first runs
whatever check that row or command makes:

| set | what opens |
|---|---|
| `equipment` | `equip`, with `inventor` listing your equipment, `persona`, `money` and `iteminfo`, as the game's equipment menu opens them. It is always your own equipment, never your automaton's. In Monstrosity it is refused, as the game's shortcut is. |
| `ability` with a list | the list with its sort window `abisortw` and, for every list but `job_traits`, its detail pane `subwindo` |
| `magic` with a list | the list with its sort window `mgcsortw` (`blusortw` for `blue_magic`) and its detail pane `subwindo` |
| `map` | the map, `map0` in its frame `mapframe`, as the main menu's map row opens it |
| `set_spells` | the blue magic set screen: `persona`, `bluehelp`, `bluinven`, `bluepoin` and `bluequip`, as the magic menu's set row opens it. The game refuses it unless blue mage is your main or support job. |
| `attachments` | the automaton's attachments: `bluehelp`, `bluinven`, `statpup` and `automato`, as the automaton menu's row opens them. The game refuses it unless puppetmaster is your main or support job. |
| `linkshell` | the linkshell list, `link5` with its info panel `link6`, as the main menu's linkshell row opens it. The row first checks what you are doing, and refuses as the row does. |
| `instincts` | Monstrosity's instincts: `mp_stat`, `level`, `mp_cstm` and `mp_mncst`, as the main menu's equipment row opens them in Monstrosity. Outside Monstrosity it is refused. |
| `synthesis` with an index | synthesis with the crystal at that inventory index: `tskill1`, with `inventor` listing what can go in and `iteminfo`, as using the crystal from the main menu's Items opens them |
| `tabula` with an index | the editor of the Maze Tabula at that inventory index: `prvdungp`, `bluehelp` and `bluinven`, as using the tabula opens them |
| `macros` | the macro editor on the macro set your keys use now: `mcrbedit`, `mcr1edit` and `mcr2edit`, as the macro set menu's edit row opens it |
| `friend` | your friend list, `friend` with its header strip `titlehan` |
| `mail` | your PlayOnline mail, `msglist` with `titlehan` |
| `mount` | the mount list with its sort window `abisortw` and its detail pane `subwindo`, as the Abilities menu's mount row opens them |
| `compare` with an item | the compare option's two panels, `itemxinf` and `itemyinf`, for an item given as `iteminfo` takes one. `iteminfo` must be open. |
| `status` | the Status screen: `persona` in the Status row's layout, which opens `level` and `statcom2` with it; in Monstrosity `mp_stat`, which opens them in turn, as the main menu's Status row opens them there |
| `region` with a kind | your area's Region Info map, `cnqframe`, with the kind's points window. A kind goes by the title the map gives it, in any case, or by its number, 1 to 4: `'Conquest'` (`conq3pts`), `'Besieged'` (`imperial`), `'Campaign'` (`allied`) or `'Colonization'` (`colopoin`). As the game's opener does, it closes the main menu, `region` and `socialme`. Refused while `cnqframe` is open. |
| `text_colors` with a category | the text color screen of one category, 1 to 4: `textcol2` and `textcol1`, as the first four rows of the configuration's text color menu open it. The game labels those rows with images, so they go by number. By the game's text, their screens' rows are: 1, the chat, from "Immediate vicinity ("Say")" to "Extremely wide area ("Yell")"; 2, from "HP/MP you recover" to "Actions you evade"; 3, from "HP/MP others recover" to "Actions others evade"; 4, "Standard battle messages", "Calls for help" and "Basic system messages" |

The index of `synthesis` and `tabula` is the item's index in your inventory,
the one `offer` takes, given as a number. The library reads the item there as
the inventory list does: `synthesis` takes a crystal and nothing else, and
`tabula` a Maze Tabula.

For a synthesis, the library runs, in order:

1. the main menu's Items row, which opens the inventory list with the game's
   own list in it
2. the game's check of whether you can act
3. the synthesis

Canceling the synthesis reopens that list, as it does for the player.

`macros` opens the editor on the current set. The game's own route through
the macro menus first picks a set, which then becomes the current one.

`open_set` is refused at once when any window of the set, or the list, is
blocked. When the game's opener does not open the set, because its own tests
or the check before it refuse it, the `error` event says the game refused it,
and says why where the library knows.

Each window of a set posts its own `opened` event as the game opens it, and
`hide` still works on one window at a time. So does `close`, except on a
set's main window, where it closes the screen as Escape does; see
[Close](#close).

### Windows open refuses

Opened by name alone, some windows crash the client, because they read what
only the game's own opener for them sets up. Others open only for a message
from the server or an NPC's script. Opened alone, they send the server a
request made of nothing, close on their first frame, or cannot be dismissed.
`open` refuses all of these, whichever addon asks:

| windows | why |
|---|---|
| `inventor`, `bank`, `itmstora`, `shop`, `bluinven`, `bluequip`, `cmbhlst`, `mp_invnt` | they read a list only their opener sets; use `open_set('equipment')` for `inventor` and `open_set('set_spells')` for `bluequip` |
| `holdtime`, `mcrledit`, `mcresed`, `loot`, `lootope`, `prvdungp`, `conquer1`, `beseige2`, `campsan`, `camparea`, `auc4`, `faqmain`, `textcol1`, `textcol3`, `conf11s` | they read a record, an index or another window's state only their opener sets; use `open_set('tabula', index)` for `prvdungp` and `open_set('text_colors', category)` for `textcol1` |
| `rmlo1`, `rmlo2`, `rmlo4` | they read the Mog House's layout, which exists only while the game lays out the room |
| `plnt1` to `plnt6` | they have no flowerpot's record, and the player's Enter on them crashes the client |
| `netwait`, `netbar`, `keypad2`, `keyselec`, `keypad3`, `roomlist`, `mogdoor`, `bankmenu` | they have no controller until the game's opener makes one; `keypad2`, `keyselec` and `keypad3` never do outside a Japanese client |
| `chocobor`, `racecard`, `mes1rcv`, `mes2frnd` | they would send the server a request made of nothing |
| `auc1`, `auclist`, `auchisto`, `ranking`, `shopmain`, `guildsho` | they open for a message from the server or an NPC's script; alone they close at once or misbehave |
| `ok`, `mp_dead` | `ok` opens empty and cannot be closed until you leave the area; `mp_dead` acts on what an earlier Monstrosity KO left in it |

Any other window whose controller does not exist at that moment is refused
the same way, since the game's open would crash on it.

Some windows act on another window's selection or state: a submenu, a panel
of what the other window has selected, a sort menu. Each opens only while one
of the windows it opens from is open. Otherwise it is refused with
"`<name>` needs `<window>` open"; where opening it alone crashes the client,
the reason also says why:

| windows | need open, any one of |
|---|---|
| `mcrselop` | `mcrselec` |
| `mcresmn` | `mcres20` |
| `gift` | `trade` |
| `auc2`, `auc3`, `aucweapo`, `aucammo`, `aucarmor`, `aucmagic`, `aucmater`, `aucfood`, `aucmeals`, `aucitem`, `wepsort`, `armsort` | `auc1` |
| `itmsortw`, `itmsort2` | `inventor`, `bank` |
| `link1` to `link4`, `link13`, `lscswin` | `link5` |
| `link7` to `link10` | `link12` |
| `flmes` | `friend` |
| `blkmain` | `blklist` |
| `merit3`, `meritinf` | `meritcat` |
| `levmeri2` | `merit2ca` |
| `guide01` | `guide00` |
| `scoption`, `acoption`, `scsibori` | `scresult` |
| `sccompar`, `sccomite`, `sccomls` | `comgenre`, `scresult` |
| `bluehelp` | `bluequip`, `automato`, `prvdungp`, `meritcat`, `merit2ca`, `fp_cat`, `fp_cat2`, `mp_mncst` |
| `bluepoin` | `bluequip` |
| `abisortw` | `ability`, `emote`, `mount` |
| `mgcsortw`, `blusortw` | `magic` |
| `subwindo` | `chatctrl`, `ability`, `mount`, `magic` |
| `iuse` | `inventor` |
| `lnowin` | `loot` |
| `mcrbedit` | `mcr1edit`, `mcr2edit` |
| `mapv3`, `scanlist` | `map0` |
| `mapv2` | `mapv3` |
| `mapscan` | `scanlist` |
| `textcol2` | `textcol1` |
| `mcrmogc` | `mcresed` |
| `cmbhwin` | `cmbhlst` |
| `mnstorag` | `equip` |
| `mp_cstm` | `mp_mncst` |
| `mp_mnnm`, `mp_mntp2`, `mp_kncst` | `mp_invnt` |
| `quest01` | `quest00` |
| `coloresu` | `cnqframe` |
| `itemxinf` | `iteminfo` |

And some cannot work alone at all. These are refused too:

| windows | why | use instead |
|---|---|---|
| `equip` | it closes again at once | `open_set('equipment')` |
| `map0`, `mapframe` | the map's menu and its frame open only together | `open_set('map')` |
| `automato` | it opens only with its set | `open_set('attachments')` |
| `mp_mncst` | it opens only with its set | `open_set('instincts')` |
| `mp_stat` | it opens only with its set | `open_set('status')` |
| `tskill1` | it opens only for a crystal | `open_set('synthesis', index)` |
| `mcr1edit`, `mcr2edit` | the game runs the three palette windows as one | `open_set('macros')` |
| `cnqframe` | it opens only with a kind's points window | `open_set('region', kind)` |
| `ability`, `magic` | they show one list at a time | name the list |
| `evitem` | it opens with what the game left in it | the key items menu |
| `emote` | it opens empty | the emote menu |

The tables are the library's own list in `engine/core.h`, and they may grow
as more windows are checked.

### Close

`close` closes one window through the game's own close routine, the one
Escape runs. It refuses:

- a prompt that has a cancel, while it asks something, even while its cancel
  is not allowed; use `cancel`
- `ok`; answer it, since the game waits for that notice to be acknowledged
- `gift` while `trade` is open, since the trade's keys would crash the client
  on a closed `gift`; `cancel('trade')` ends the trade
- the windows listed under [Hide and block](#hide-and-block) as unable to
  be blocked or closed

Only the game's own state says whether a prompt is asking, so that refusal
comes as an `error` event once the game looks at it. A prompt asking nothing
closes like any window. `link5` showing your linkshells is one, and closing
it takes `link6` and the action menus with it, as Escape does.

`confyn` and `comment` have no cancel and close like any window. Closing
`comment` sends its message, as Escape does in the game.

Some closes take other windows with them:

- Closing `map0` closes the map as Escape does: the map's other windows close
  with it, and its frame, `mapframe`, fades out and closes.
- Closing `mcr1edit` or `mcr2edit` closes the macro editor as Escape on a
  palette does: `mcrbedit`, `mcr1edit` and `mcr2edit` close together, and
  the title strip steps back to the macro menu's.
- Closing `persona` or `mp_stat` while `statcom2` is open closes the Status
  screen as its Escape does: `statcom2` closes, and its own close takes
  `persona` and `mp_stat` with it.
- Closing `textcol1` closes `textcol2` first, then `textcol1`, as Escape on
  the text color screen does.
- Closing `mp_mncst` runs its Escape, which closes `mp_stat`, `mp_cstm` and
  `mp_mncst`.
- Closing `cnqframe` runs its Escape too. Its points windows and the
  Colonization panes close, `coloresu` among them, and the map fades out and
  then closes itself, so its `closed` event comes a moment later. A close
  while it is already fading out, and a block's close, close it at once.

A window that opens only from another never outlives that window. These are
the windows listed under [Windows open refuses](#windows-open-refuses) as
needing another window open. In the game, the player cannot leave the window
under one of these while it is up, because Escape leaves the child first.

Closing a parent does the same. Each child whose other parents are all closed
is closed first, through its own close and with its own `closed` event, and
then the parent. `close('quest00')` with `quest01` up closes `quest01`, then
`quest00`. A child that another of its parents keeps stays open: `sccompar`
stays while `scresult` is open, even after `comgenre` closes.

Where the game's own close of the parent closes the child, the library leaves
it to the game:

- `auc1` closes its category lists, `auc3`, `wepsort` and `armsort`
- `inventor` closes `itmsortw`, `itmsort2` and `iuse`
- `link5` and `link12` close their submenus
- `friend` closes `flmes`, and `blklist` closes `blkmain`
- `meritcat` closes `meritinf`, and `merit2ca` closes `levmeri2`
- `scresult` closes `scoption` and `acoption`
- `bluequip` closes `bluepoin`
- `ability`, `emote` and `mount` close `abisortw`, and `magic` closes its
  sort window
- `ability`, `mount`, `magic` and `chatctrl` close `subwindo`
- `scanlist` closes `mapscan`
- the Escapes above close `map0`'s, the palettes', `textcol1`'s, `mp_mncst`'s
  and `cnqframe`'s

`ability` closes `abisortw` and `subwindo` only when it opened them itself,
as `open_set('ability', list)` does. Any child the parent's close leaves up,
including one `ability` did not open, is closed right after the parent.

When a block closes a parent, all of this happens too.

## The compass

The compass at the lower left, with the Vana'diel clock and the weather icon,
is not one of the 366 windows: the client draws it separately. The library
covers it anyway, under the name `compass`. `hide`, `unhide`, `move` and
`reset` work on it as on a window:

```lua
ui:hide('compass')
ui:unhide('compass')
ui:move('compass', 200, 900)    -- top-left corner of its box
ui:reset('compass')
```

Its box is 88 by 42 UI pixels, and `ui:info('compass').rect` tells you where
it is right now. Left alone, the game keeps the box just above the chat log,
moving it whenever the log grows or shrinks. Once you move it, it stays where
you put it until you reset it.

The game shows and hides the compass itself, so `block` cannot stop it
opening. On the compass, `close` and `open` do what `hide` and `unhide` do,
and `block` hides it under a separate hold: a compass hidden by `block` stays
hidden after `unhide`, until `unblock`.

Its `closed` and `opened` events report the game's own hiding and showing: it
hides the compass during a cutscene and while the map is open, and shows it
again afterward.

The clock inside it is switched with the game's own `/clock on` and
`/clock off`; the library leaves it alone.

## The target arrows

The arrow over your target, and the second arrow shown while you pick the
target of a command, are drawn by the target window, `targetwi`. Hiding
`targetwi` hides the window and leaves the arrows on screen. To hide the
arrows, use the name `targetarrow`:

```lua
ui:hide('targetarrow')      -- no arrows; the target window stays
ui:unhide('targetarrow')
```

`targetarrow` is not a window either. `block` and `close` hide the arrows, and
`unblock` and `open` show them again. As on the compass, a `block` is separate
from `hide`: arrows hidden by `block` stay hidden after `unhide`. `move` and
`resize` are accepted and do nothing, because the arrows follow the target.

`ui:info('targetarrow')` and `ui:rects()` give no box for it. Its `opened` and
`closed` events say when the game starts and stops drawing an arrow.

Blocking `targetwi` takes the arrows away too: the game computes them as part
of the target window, and a blocked window never opens.

## Macro keys

Ctrl+1 through Ctrl+0 and Alt+1 through Alt+0 run the game's macros, and the
macro bar is only a display of them: blocking the bar windows leaves the keys
working. To take those keys for your own Windower binds, block the macros
themselves:

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

Every event carries `e.event`. Most also carry `e.name`, the window;
`pending` and `resync` do not.

The `opened`, `closed`, `covered`, `uncovered` and `blocked` events for
`ability` also carry `e.category`, the game's number for the list, and
`e.category_name`, its name from the table under [Lists](#lists). Naming the
list as a third argument to `on` or `off`, as in the example, keeps the
handler to that list.

| event | when |
|---|---|
| `opened` | the window is open now. The game re-opens some windows while they are open; the Region Info map's points window is re-opened every frame. Those opens post nothing, so `opened` and `closed` alternate for each window. The one exception is `ability`, which posts `opened` again when it changes to another list |
| `closed` | the window is gone |
| `covered` | another window went over it; it is still open underneath |
| `uncovered` | it came back out |
| `blocked` | the game tried to open a window some addon blocked; `e.mine` is true when the block is yours, and `e.by` lists every addon holding one |
| `error` | the game refused something you asked for, or one of your own callbacks raised. `e.verb` names the call, or is `'callback'`. `e.name` is what the call named: the window, the set for `open_set`, the group for `move_group` and `reset_group`, or the setting for `set_setting`. It is `''` for a call that names none. `e.reason` says why |
| `cursor` | the game's cursor in an open window moved to row `e.row`; for `query` and `arealist`, `e.row` is the index into the list `options()` returns |
| `page` | an NPC dialogue box, `rem4line` or `rem4li2`, shows a new page; read it with `options(e.name)`. When the dialogue ends, the box's `closed` follows |
| `pending` | a question with no window to show it appeared or went away: a party invite, a delivery-box session, or a blocked question of the windows listed under [Questions with no window](#questions-with-no-window). `e.what` is `'invite'`, `'post'` or the window's name, `e.pending` true or false |
| `resync` | events were lost because your addon fell more than 4096 events behind; `e.dropped` says how many. Re-read `opened()`, `info()` and `pending()` to catch up |

Callbacks run on the frame after the event, so a window may already be gone by
the time `opened` reaches you; check `ui:info(name).open` before acting on it.
A window already open when your addon loads sends no `opened`, so check what
is open on load.

A callback that raises never stops the others. Its first raise comes back to
you as an `error` event with `e.verb == 'callback'`; later raises are only
counted, and `hideui.debug(true)` shows the count. See [Status and
debugging](#status-and-debugging).

## Read a window

```lua
local p = ui:info('targetwi')     -- nil and a reason for a name that is no window

-- state
p.name                -- the window's name
p.open                -- true/false
p.focused             -- has the keyboard
p.covered             -- open but under another window
p.docked              -- the game keeps it against another window
p.layer               -- the draw layer the game's window table gives it

-- holds
p.hidden, p.blocked   -- by any addon
p.hidden_by, p.blocked_by   -- names of the addons holding a hide or a block on it
p.blockable           -- whether block is allowed on it
p.mine                -- {hidden, blocked, blocked_categories}: what your own addon holds on it

-- place and size
p.rect.x, p.rect.y, p.rect.w, p.rect.h   -- while open; p.rect.right and p.rect.bottom are x + w and y + h
p.default             -- while open: the frame the game's layout gives the window, in the form of p.rect
p.origin              -- while open: {x, y}, the point the game places the window by; for the chat logs and
                      --   the party list, which grow up from their bottom edge, it is not the frame's top-left
p.resize              -- {holds, min_rows, max_rows}; see Resize
p.memory              -- the remembered position and size, as remembered() lists them

-- contents, while open
p.items               -- the number of rows the game's cursor moves through
p.cursor              -- the row the game's cursor is on, 1-based; for query and arealist, the index into their list
p.top                 -- query and arealist: the index of the first row on screen; query shows three rows, arealist five
p.elements            -- a list of {type = 'item', x = 44, y = 836, w = 132, h = 16, text = 'Yes'};
                      --   type is 'frame', 'item', 'cursor' or 'other'; each element has only the fields it can have,
                      --   and one with w and h also has right and bottom, x + w and y + h
p.elements_truncated  -- true when the window has more than 128 elements; p.elements lists the first 128

-- ability only
p.category            -- while open: the list's number, as under Lists; p.category_name is its name
p.blocked_categories  -- the lists any addon has blocked

p.detail              -- internals; present only while hideui.debug(true) is on, see Status and debugging

ui:opened()           -- { 'logwindo', 'partywin', ... }: every window open now
ui:focused()          -- the name of the window with the keyboard, or false
ui:list()             -- a table keyed by window name, each {name, open, hidden, blocked, blockable, layer, moved, resized},
                      --   the fields as info() has them; compass and targetarrow have no layer
ui:groups()           -- { chat_log = {anchor, anchor_open, origin, members, waiting}, ... }, see below
ui:remembered()       -- { targetwi = {position = {x, y, owner, mine}, size = {rows, owner, mine} or {w, h, owner, mine}}, ... };
                      --   a position a group move placed also has group, the group's name
ui:rects()            -- { logwindo = {x = 16, y = 898, w = 1774, h = 166}, ... }: every open window's frame
```

Each group in `groups()` is:

| field | meaning |
|---|---|
| `anchor` | the group's anchor window |
| `anchor_open` | whether it is open |
| `origin` | while the anchor is open, its origin as the game keeps it. The chat log and the party list grow up from their bottom edge, so for `chat_log` and `party_list` this is not the frame's top-left; `info(anchor).rect` gives that |
| `members` | the group's other windows, by name |
| `waiting` | a group move waiting for the anchor to open: `{x, y, owner, mine}` |

Three calls look alike:

- `list()` flags each window as moved or resized by any addon.
- `remembered()` gives the positions and sizes the library has put on
  windows, and who put them there.
- `layout()`, under [Save a layout](#save-a-layout), is what your own addon
  asked for.

## Prompts

Some windows are the game asking the player something: an NPC's dialogue and
choice lists, a yes or no, a number, a name, a trade. If you hide one to draw
your own, you also have to answer it, or the player is stuck. Each reply does
what the window's own confirm or cancel does in the game.

These are the prompt windows, by the section that covers them:

| section | windows |
|---|---|
| [The NPC choice list](#the-npc-choice-list) | `query` |
| [The NPC dialogue](#the-npc-dialogue) | `rem4line`, `rem4li2` |
| [Yes or no](#yes-or-no) | `comyn`, `merityn`, `shopbuy`, `shopsell`, `resyn`, `resynus`, `resynde`, `resynfr` |
| [The KO window](#the-ko-window) | `dead` |
| [Numbers](#numbers) | `moneyctr`, `itemctrl` |
| [Text](#text) | `passinpu`, `stringdl`, `scname`, `msgline`, `comment`, `link11` |
| [Trade](#trade) | `trade`, `handover` |
| [The area list](#the-area-list) | `arealist` |
| [Everything else](#everything-else) | `prtyjoin`, `link5`, `delivery`, `post1`, `post2`, `gmtell`, `sortyn`, `evitem01`, `confyn`, `bankmenu`, `maplist`, `mogdoor`, `mp_dead`, `ok` |

Every prompt is read and answered with the same three calls, and a trade adds
two more. Not every prompt accepts all three; each section says which:

```lua
local q = ui:options(name)    -- what the prompt is offering; nil once it is gone
ui:answer(q, value)           -- the player's choice
ui:cancel(q)                  -- the player's cancel
```

Every table `options()` returns carries the prompt's `name` and an `id` for
this one open of it. For a party invite or a delivery-box session, the `id`
stands for the invite or the session. The windows that keep their question
when you block them, listed under [Questions with no
window](#questions-with-no-window), add `blocked`, true while the question
waits with no window. Each section below gives the rest of the fields, and
[The game's text](#the-games-text) says how their strings come back.

A reply carrying an old `id` is refused, so it cannot land on a prompt that
has replaced the one you meant. So is a second reply while the first is still
queued.

When you don't care which open it is, `ui:answer(name, value)` and
`ui:cancel(name)` work too. To name one open without the table, pass its id:
`ui:answer(name, value, id)` and `ui:cancel(name, id)`.

A few windows take no reply in some of their uses, because the game takes none
there either: a list the player opened only to look at, another player's
bazaar message. `options()` still reads them.

### The NPC choice list

Every list of choices an NPC's dialogue offers is the window `query`: a yes or
no, the home point menus, a treasure chest's items. The game's own
confirmations outside dialogue are a different window, `comyn`, under [Yes or
no](#yes-or-no). Hide `query`, draw the options your own way, and answer with
the one the player picks:

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
        if ui:answer(q, q.options[pick].value) then q = nil end   -- one answer per list
    elseif dik == 1 and q.cancellable then                    -- Escape
        if ui:cancel(q) then q = nil end
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

Answer with an option's `value`, never its position: "Yes" is not always 1.

### The NPC dialogue

An NPC's dialogue is a series of messages, each shown as one or more pages.
Every page appears in two boxes at once, `rem4line` over the chat log and
`rem4li2` over the second chat log. The game sends the player's keys to
whichever it makes active, usually `rem4li2`.

The two names are one dialogue:

- hiding or unhiding either hides or unhides both
- a block of either is refused for both
- an answer or a cancel through either moves both boxes on, or ends the
  conversation in both

Each name still reports its own box, with its own `options()` and events, and
both report the same `id`. Each box sits over its own chat log, so a move or
a resize places one alone.

Hide either one, and listen and answer on either one:

```lua
ui:hide('rem4line')                              -- rem4li2 with it

-- draw_my_dialogue and hide_my_dialogue are yours to write
local d                                          -- the page on screen

local function show()
    d = ui:options('rem4line')                   -- nil once the dialogue is over
    if d then draw_my_dialogue(d.text) end       -- its lines joined by '\n'
end

ui:on('page', 'rem4line', show)
show()                                           -- a dialogue already up when the addon loaded

ui:on('closed', 'rem4line', function()
    d = nil
    hide_my_dialogue()
end)

windower.register_event('keyboard', function(dik, down)
    if not d or not down then return end
    if dik == 28 and d.waits ~= 'timer' then     -- Enter, on a page that waits for it
        ui:answer('rem4line')                    -- the next page arrives as a page event
    elseif dik == 1 and d.cancellable then       -- Escape
        ui:cancel('rem4line')
    end
end)
```

The first page of a dialogue sends `page` too.

`ui:options(name)` returns `nil` while the box shows nothing, and otherwise:

| field | meaning |
|---|---|
| `text` | the page the box shows; the first page of a message starts with the NPC's name, `Name : `, as the game draws it |
| `waits` | what moves the page on: `'key'`, `'timer'` or `'key_or_timer'` |
| `seconds` | the time a timed page has left |
| `auto` | true while the NPC's script moves each page on by itself |
| `more` | true while another page of this message follows |
| `queued` | how many messages wait behind this one |
| `cancellable` | whether the NPC's script allows backing out |

`ui:answer(name)` moves on exactly one page, even where the game's own Enter
would skip ahead. It is refused when `waits` is `'timer'`.

`ui:cancel(name)` backs out of the conversation as the game's Escape does,
when the script allows it, and closes both boxes.

While hidden, a box takes no keys, and clicking the screen no longer moves it
on; the Enter and Escape presses your addon reads do nothing in the game.

### Yes or no

The game's confirmations are one kind of window under several names:

| window | asks |
|---|---|
| `comyn` | most confirmations, such as logging out or using an item |
| `merityn` | about merit points |
| `shopbuy`, `shopsell` | to confirm a purchase or a sale |
| `resyn`, `resynus`, `resynde`, `resynfr` | whether to accept a raise; one name per client language |

```lua
local q = ui:options('comyn')     -- {name, id, text, default, cancellable, hold}
ui:answer(q, true)                -- yes; false is no
ui:cancel(q)                      -- refused when the question cannot be canceled
```

`text` is the question, and `default` is true when the window opens with the
cursor on Yes. The shop confirmations and the raise prompt carry no text of
their own: the shop window says what is being bought or sold, and the raise
prompt draws its own caption.

`hold` is true for a question the player confirms by holding the key.
`answer` confirms it at once.

When a reply leads the game to ask a second question, the second arrives as a
new prompt with a new `id`.

### The KO window

`dead` is the window the game shows when the player is knocked out.
`ui:answer('dead')` does what the player's confirm on it does: the game asks
"Return to home point?" through `comyn`, which you answer as above.

```lua
local k = ui:options('dead')      -- {name, id, seconds, askable}
ui:answer(k)
```

`seconds` is the time on its countdown. In some areas the countdown stays at
0 and the game never sends the player home by itself.

`askable` says whether the question can be asked now; it cannot while
`comyn` has a question up. `cancel('dead')` is refused: the window has no
way out.

### Numbers

`moneyctr` asks for an amount of gil and `itemctrl` for a number of items. The
game uses them for the delivery box, trades, shops, the bazaar, the auction
house, the Mog House storage and synthesis.

```lua
local n = ui:options('itemctrl')  -- {name, id, value, min, max, item, blocked}
ui:answer(n, 12)                  -- refused outside min to max
ui:cancel(n)
```

`value` is the number the box shows, and `min` and `max` the numbers it takes.
`item` is the item's id, its number in Windower's `res.items`, when the count
is for an item the player picked from the inventory. This is not the `item`
that `offer` takes in a trade, which is an inventory index.

### Text

The windows that take typed text take the game's own bytes: plain ASCII goes
in as it is, and anything else your addon converts with
`windower.to_shift_jis` first.

Each window's limits are checked before anything is sent, and a text the game
would refuse is refused with the reason.

| window | `ui:answer(name, ...)` | `ui:cancel(name)` |
|---|---|---|
| `passinpu`, an NPC's password | the text, up to `max_length` bytes | answers with no text |
| `stringdl`, the delivery box's recipient | a name of 3 to 15 letters, A to Z. When `mode` is `'confirm'`, the window asks about a delivery slot and takes `true` or `false` instead | backs out, as the window's own cancel |
| `scname`, the player search by name | a name of up to 15 letters, A to Z; starts the search | closes it |
| `msgline`, a PlayOnline friend message | the message, 1 to `max_length` bytes: 299, or 15 in some modes; refused while the previous one is still on its way | closes it, sending nothing |
| `comment`, the bazaar, search and linkshell messages | `{'first line', 'second', 'third'}`, up to 40 bytes each; a line left out is empty. Sends the message. When `mode` is `'view'`, showing another player's message, it takes none | refused: no cancel. Outside `'view'`, Escape and `close` both send the message |
| `link11`, a new linkshell | `{name = 'MyShell', color = {r, g, b}}`: a name of 1 to 19 letters, A to Z, and each of `r`, `g` and `b` 0 to 15. `color` can be left out | backs out |

`options(name)` gives each window's limits and what it holds now:

| window | `ui:options(name)` |
|---|---|
| `passinpu` | `{max_length}` |
| `stringdl` | `{mode, prompt, min_length, max_length}`; `mode` is `'name'` or `'confirm'` |
| `scname` | `{max_length, text}` |
| `msgline` | `{mode, max_length, recipient}` |
| `comment` | `{mode, lines, category}`; `mode` is `'bazaar'`, `'view'`, `'search'` or `'linkshell'`, `lines` the three lines as `{text, raw}`, `category` for a search comment |
| `link11` | `{max_length, text, color}`, `text` the name it holds and `color` `{r, g, b}` |

### Trade

`trade` is a trade with another player and `handover` one with an NPC. Both
take the same calls:

```lua
ui:offer('trade', item, count)    -- item: its inventory index, as windower.ffxi.get_items(0) gives it, not its id
ui:offer('trade', 'gil', amount)  -- the gil; 0 takes it out
ui:withdraw('trade', item)        -- take that item back out
ui:answer('trade')                -- OK
ui:cancel('trade')                -- cancel the trade
```

`offer` puts the item in the first free slot of the window's eight. These are
refused, with a reason, before anything reaches the game:

- a ninth item, or an item already offered; another stack of the same item
  is another item
- a count below 1 or above what that inventory slot holds; gil below 0 or
  above what the player has
- an item the game grays out in its own list for that window
- an item outside the main inventory
- `withdraw` of an item that is not offered
- any change once the player's own OK is in
- for `trade`, more than one change a second, and for `handover`, more than
  one OK a second: the game itself sends no faster. The reason says how long
  to wait.

`ui:options('trade')` returns:

| field | meaning |
|---|---|
| `partner` | the other player's name |
| `mine`, `theirs` | each `{gil, items}`; an item is `{slot, id, count}`, with `id` its number in `res.items`; yours also carry `item`, the inventory index, and `pending = true` until the server takes the change |
| `ok_mine`, `ok_theirs` | whether each side has pressed OK |
| `next_change` | seconds until the next change can go, 0 for now |
| `gray` | the inventory indexes the game grays out, so you can gray them too |

An item still marked `pending` counts as offered, and it cannot be withdrawn
until the server takes it.

`ui:options('handover')` returns `{npc, items, gil, next_ok, gray}`, with
`items` in the same form as a trade's `mine.items` and `next_ok` the seconds
until the OK can go. Nothing reaches the NPC until the OK, which sends the
whole offer.

No event reports a change in a trade; re-read `ui:options('trade')` while it
is open, each frame or on your own schedule.

### The area list

`arealist` is the list of areas an NPC asks the player to pick from, and the
same list the search menu shows. `ui:options('arealist')` returns
`{mode, level, pending, rows}`:

- `mode` 1 or 2: an NPC opened the list and waits for a reply, so `answer`
  and `cancel` work. Any other value: a menu the player opened, which the
  library can only read.
- `pending` is always true. It is unrelated to `ui:pending()`.
- `level` is 0 at the top of the list; inside a region it is that region
  row's `id` without its minus sign.
- `rows` is every row the list shows, in order, each `{text, id, kind}`.

`kind` is one of:

- `zone`: an area; `id` is its number in Windower's `res.zones`
- `region`: a heading the player can open; `id` is the region's number, negated
- `current_area`, `current_region` and `all`: the list's rows for the current area, the current region and all areas
- `other`: any row that is none of the above

A row may also carry `label`, the text beside it, with its bytes in
`label_raw`, or `count`, a region's number of zones.

`ui:answer('arealist', id)` takes a zone row's `id`, and `ui:cancel` closes
the list with no zone. A blocked list has no rows, and `answer` then accepts
any zone id, 0 to 511.

### Everything else

| window | `ui:options(name)` | `ui:answer(name, ...)` | `ui:cancel(name)` |
|---|---|---|---|
| `prtyjoin`, a party invite | `{inviter = 'Somebody', alliance = false}` | `true` accept, `false` decline | declines |
| `link5`, the linkshell list | `{slots = {{slot = 1, name = 'MyShell'}, ...}}` | a `slot` from the list | closes it, no choice |
| `delivery`, the outgoing box | refused | refused | ends the session; the windows close when the server answers |
| `post1`, `post2`, the incoming box | refused | refused | ends the session; the windows close when the server answers |
| `gmtell`, a GM's question | `{gm, title, options = {{text, value}, ...}, cancellable}` | an option's `value` | declines |
| `sortyn`, dropping an item or sorting | `{text, title, default, cancellable}` | `true` or `false` | as the window's cancel |
| `evitem01`, using a key item | `{text, default, cancellable}` | `true` or `false` | as the window's cancel |
| `confyn`, restoring a config page's defaults | `{default}` | `true` restores, `false` leaves them | refused: no cancel |
| `bankmenu`, which Mog Safe | `{containers = {{id = 1, available = true}, {id = 9, ...}}}` | a container `id` | backs out |
| `maplist`, the map list | `{rows = {{text, value, kind}, ...}, top, level}`. A map row has `zone` and `map`, and a row may carry `label` or `count`. `level` is `'top'`, `'region'` or `'zone'`: the list shown | a row's `value`. A map row picks that map, the closing row closes the list, and a row whose `kind` is `'list'` opens its sub-list. Each list is a prompt of its own, with a new `id`, and when blocked, a `pending` event of its own | closes the list at its top level; inside a sub-list, goes back up a level |
| `mogdoor`, the Mog House door | `{title, level, options, areas}`. `level` is `'top'` or `'areas'`: the list the window shows. Each entry of `options` and `areas` is `{text, kind, enabled}`, plus `value` on a row that is an exit; the stay row, the row that opens the areas, and the floor row while it has no exit have none. An area also has `zone` | an enabled `value` from either list | closes it, as Escape does |
| `mp_dead`, the Monstrosity KO | `{options = {{text, value}, ...}, seconds}` | `1` or `2` | refused: no cancel |
| `ok`, the notice shown when the connection is lost | `{text}` | no value; acknowledges it | refused: no cancel |

### Questions with no window

Some questions can be waiting with no window to show them:

- a party invite, before the player opens the party menu
- a delivery-box session whose windows are blocked
- a question in one of the windows listed below, when you blocked that window

The `pending` event tells you when one appears and when it ends.
`ui:pending()` lists what is waiting, and each entry is a table that goes
straight into `answer` and `cancel`:

```lua
local w = ui:pending()
if w.invite then ui:answer(w.invite, true) end     -- w.invite.inviter, w.invite.alliance
if w.post then ui:cancel(w.post) end               -- w.post.box is 'delivery' or 'post1'
if w.moneyctr then ui:answer(w.moneyctr, 1000) end -- a blocked number box, its options() fields included
```

These windows keep their question waiting when you block them, as the table
under [Hide and block](#hide-and-block) lists: the number boxes, `sortyn`,
`evitem01`, `bankmenu`, `maplist`, `ok`, `mogdoor` and `msgline`.

Such a question starts with the window's `blocked` event and a `pending`
event. It ends with a `pending` event with `e.pending` false, which every
addon gets except the one whose reply ended it.

The game drops each question when it would have closed the window:

- the number boxes, `sortyn`, `evitem01` and `maplist`: when the window that
  asked closes or moves on
- `mogdoor`: when the player leaves the Mog House or uses the door again
- `msgline`: when another message is set up, or `msgline` opens
- `bankmenu`: never; it waits for an answer or a cancel, and the game opens
  no other Mog Safe picker until then
- `ok`: when the game sets up another notice or the notice is acknowledged

While a number box is blocked, the window that asked for the number stays open
and keeps taking the player's keys. An answer is refused while that window's
cursor is off the row that asked, or while it is busy with another step.

A blocked `moneyctr` asked by an auction house bid takes only a cancel,
because a new search can replace the item it asks about.

A number box asked by game code the library does not recognize takes neither
answer nor cancel; that can happen after a game update. Unblock the box, and
the game shows its own the next time it asks.

### The game's text

Every string the library returns from the game is UTF-8, the form Windower's
text objects draw. Beside each one sit the game's own bytes, the Shift-JIS
form `windower.add_to_chat` wants:

| string | its bytes |
|---|---|
| `text`, in a question, an option, a page, a line | `raw` |
| `inviter`, `gm`, a linkshell's `name` | `raw` |
| `partner` | `partner_raw` |
| `npc` | `npc_raw` |
| `recipient` | `recipient_raw` |
| `prompt` | `prompt_raw` |
| `label`, beside an area list or map list row, and a setting's | `label_raw` |

A question, a page, an option and a title are the texts the game draws in
color. Each of them also carries `segments`: the same text as a list of runs,
each `{text, raw, color}`. `color` is `'default'`, `'green'`, or
`'color<n>'` for a color the library has no name for.

Each of those texts also carries `undecoded`, the number of characters the
library could not decode. Each such character shows as `?` in `text`.

## Settings

Every control the player finds on a page of the game's Config menu is a
setting your addon can read and change, with the page closed: the volumes,
the chat filters, the text colors, the keys and the rest. Block the Config
windows, and the player can no longer change the settings, but your addon
still can.

```lua
ui:setting('music_volume')                                -- 100
ui:set_setting('music_volume', 40)
ui:set_setting('weather_effects', false)
ui:set_setting('shadows', 'high')
ui:set_setting('color_say', {r = 255, g = 255, b = 255})
ui:set_setting('key1_autorun', 19)                        -- a DirectInput key code, R's

for _, s in ipairs(ui:settings()) do                      -- in the Config menu's order
    print(s.name, s.page, s.kind, s.value)
end
```

`set_setting` changes a setting as its page does when the player changes it,
and the game applies it at once. The game keeps the values with the rest of
the player's settings and saves them as the player logs out, as it does with
the changes made on its pages.

`ui:setting(name)` returns one setting's value, and `ui:settings()` lists
every setting, page by page in the Config menu's order, each entry a table:

| field | meaning |
|---|---|
| `name` | what `setting` and `set_setting` take |
| `page` | the Config window that shows it, a name under [All window names](#all-window-names) |
| `kind` | what its value is, in the table below |
| `value` | its value now |
| `default` | the game's default for it; nil for `character_information`, which the server keeps |
| `min`, `max` | the values a number or a key takes; for a color, each channel's |
| `choices` | a choice's names, in its page's row order |
| `label` | the game's text for it, nil where the game has none; see [The game's text](#the-games-text) |

| kind | value |
|---|---|
| `number` | a whole number from `min` to `max` |
| `bool` | `true` or `false`: a row whose buttons are ON and OFF, `true` for ON |
| `choice` | the name of one of its `choices`: a row with other buttons, each choice named after one |
| `color` | `{r = red, g = green, b = blue}`, each a whole number from 0 to 255 |
| `key` | a DirectInput key code from 0 to 255, such as 17 for W |

Both reads return what the game holds at the moment you call.

A setting's name is its label on its Config page, such as `music_volume` or
`pc_armor_display`. In the game's lists, such as the chat filters, the text
colors and the keys, a row's name is the row's own text instead:
`filter_attacks_by_you` or `key1_autorun`, for example. Names are lower case,
but `setting` and `set_setting` take a name, and a choice's name, in any
case. Each name is listed under [All settings](#all-settings).

What the values mean:

- a `bool` on the chat filter page, `cfilter`: `true` filters those messages
  out. A `number` there is the row's state, numbered from 0 in the order the
  page steps through the states each time the player picks the row.
  `filter_system_level` is the page's level, 0 to 3
- the battle effects filter, `fxfilter`: `true` filters those effects out
- the log window settings, `conf11s`: `true` shows those messages in that log
  window, `log1_` in the first and `log2_` in the second
- `auto_disconnect` is in minutes, and -1 is off; setting it to 0 turns it off
  too
- `log_window2_width` and `log_window2_resize_time` take -1 to copy log
  window 1's value: the game replaces the -1 with that value at once, so
  `setting` then reads the number, and later changes to log window 1 do not
  carry over
- `character_information` is a request to the server, sent as its page sends
  it, and reads back its new value once the server has answered

`style_lock` is a request to the server too, and its value changes only once
the game sends the request. The game allows one change every 9 seconds.
Asking sooner shows the game's own message that you must wait longer, leaves
the value as it was, and posts an `error` event saying how many seconds are
left. The game can also turn a request down without a message, which leaves
the value and posts an `error` event as well. The server's answer keeps the
value matching the lock.

Settings the game keeps together, such as a page of chat filters, change one
at a time: a set changes its own setting and leaves the others as they are.

`min` and `max` are the game's own limits, and for a few settings they reach
past what the page offers:

- `character_models_displayed` also takes 51, above the page's 25 to 50
- `log_window2_width` also takes -1, below the page's 0 to 100
- `log_window2_resize_time` also takes -1 to 2, below the page's 3 to 30
- `auto_disconnect` takes every minute from 1 to 60, where the page steps by
  ten
- a color takes every value from 0 to 255, where the color editor steps by
  four

`set_setting` returns `nil` and a reason, and changes nothing, for

- a name no setting has
- a number that is not whole, or is outside `min` to `max`
- a name that is not one of a choice's `choices`
- a color channel that is a fraction or outside 0 to 255
- a set while the player is not in the game: at the title screen and the
  character list, the game holds no player's settings

While the player is not in the game, `setting` and `settings` return `nil`
and a reason as well.

A value the setting already has is not refused.

Two mistakes raise instead, as misuse does: a value of another type than the
setting's kind, and a color that is not a table of the three numbers `r`, `g`
and `b` alone. See [Return values and errors](#return-values-and-errors).

Five pages work on a copy of their settings and write the whole copy back as
they close: `cfilter`, `fxfilter`, `conf11s`, `k1assign` and `k2assign`.
`conf3win` does the same with `character_information`. A set made while one
of them is open would be undone as it closes, so the game's next frame
refuses it with an `error` event whose reason names the page. Set it once the
page has closed.

The two key pages count as one: they edit one copy of every key, so a `key1_`
or `key2_` setting is refused while either page is open.

## Save a layout

To bring positions and sizes back after a reload, save `ui:layout()` with
Windower's config library and pass it to `ui:apply()` on load.

Ship `data/` with a file in it, such as a default `settings.xml` as the
example addon does. Config writes `data/settings.xml`, and a write into a
missing folder freezes the client. Git and zip downloads drop empty folders,
which is why `data/` needs a file in it.

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

## More than one addon

Several addons can use the library at once. The engine in charge is the
`_HideUI.dll` of whichever addon loaded first; every later addon's copy talks
to it, and once all of them have unloaded the next addon to load starts afresh
with its own.

Hides and blocks are held per addon: a window hidden or blocked by two addons
stays that way until both let go, and the same goes for the macro keys.

Moves are not. The library keeps one position per window, so a window moved
by two addons sits where the later move put it. When that addon unloads, the
window goes back to where the game puts it, not to the earlier addon's spot.

## Status and debugging

```lua
hideui.version()      -- 'hideui 0.11.0'; goes in a bug report
hideui.debug(true)    -- print your addon's library failures to chat; off, it prints nothing
hideui.status()       -- the fields below
ui:status()           -- the same, with .dropped for this handle only
```

`hideui.status()` describes the engine for the whole game:

| field | meaning |
|---|---|
| `ok` | whether the engine installed |
| `state` | `'installed'`; `'failed'` when the install failed; `'closing'` while the engine takes its hooks out after the last handle closed; `'idle'` when it is not installed |
| `engine` | the engine's version, such as `'0.11.0'` |
| `handles` | how many handles are open, across every addon |
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
returns `nil` and a reason when the library cannot install; the install
example above prints it and returns. The reasons:

- `hideui_daemon.dll` is missing beside `_HideUI.dll`
- the daemon another addon loaded first is older than yours
- the engine in charge is older than your addon's copy and lacks a call
  yours needs; see [Updating](#updating-the-library-in-your-addon)
- a game patch changed the code the library relies on

## Updating the library in your addon

When your addon's copy of the engine is older than the engine in charge, your
addon works unchanged. When yours is newer and needs a call the older engine
does not have, `new` returns `nil` and names the engine in charge by its path,
so you know which addon to update. A field the older engine does not know is
missing from what you read.

To update, unload every addon that uses the library and wait a few seconds.
Replace `hideui.lua` and `_HideUI.dll` in your `libs/`, then load your addon
before the others, so its copy becomes the engine in charge. Never write over
`_HideUI.dll` while it is loaded.

Replace `hideui_daemon.dll` with the game closed. If the daemon another addon
loaded is older than yours, `new`'s reason names that daemon's path: close
the game, copy your `hideui_daemon.dll` over that file, and start the game
again.

## Under the hood

The library finds two things in the running game: its list of windows, and
fourteen of its routines, which it hooks. They are the routines that:

- open, close and draw windows
- pass keys and the mouse to them
- change their layouts
- run the UI each frame
- draw the compass and the target arrows
- let the macro keys run

The game draws, runs, orders and focuses a hidden window as it would the
window shown; the library only keeps what that draw paints off the screen.

The library makes every change from inside the game's own thread on the
game's next frame, which is why a call only queues the change and returns at
once. The settings are read where the game keeps them and set through the
routines the Config pages call.

The patches on the game's routines belong to `hideui_daemon.dll` rather than
to the engine, so that the engine can be unloaded and replaced while the game
runs. The daemon stays until the game closes, which is also why an older
daemon another addon loaded cannot be swapped for yours.

Building it: [`engine/README.md`](engine/README.md) and
[`daemon/README.md`](daemon/README.md).

## All window names

Every name the library accepts, with a description where one is known, taken
from the window's own captions or the menu it opens from. A blank means it has
not been identified yet; the name is still valid.

| name | window |
|---|---|
| `ability` | job abilities, pet commands, weapon skills and job traits: one window, `info` says which |
| `abimenu` | abilities menu (traits, abilities, weapon skills) |
| `abiselec` | ability category list |
| `abisortw` | ability sort menu |
| `acoption` | chat mute list option |
| `alarm` | party invite alert settings |
| `allied` | allied notes total on the campaign map |
| `arealist` | area list (the search menu's areas, and the NPC prompt) |
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
| `bankmenu` | which Mog Safe: Mog Safe or Mog Safe 2 |
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
| `charlnk` | a chat log line's actions: /tell, search, target, party, Assist channel, copy |
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
| `coloresu` | one Colonization region's status, beside the map |
| `comgenre` | search comment category |
| `comment` | bazaar, search and linkshell message editor |
| `commenu` | friends and emotes menu |
| `compass` | the compass with the clock and weather; not a window, see [The compass](#the-compass) |
| `comyn` | confirm yes/no |
| `conf11l` | log routing: one of `conf11m`'s two sets |
| `conf11m` | log routing menu |
| `conf11s` | log routing: which messages each log window shows |
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
| `dead` | KO window |
| `delivery` | outgoing delivery box |
| `emote` | emote list |
| `equip` | equipment window |
| `eventtim` | event timer |
| `evitem` | a status menu screen |
| `evitem01` | key item use confirm |
| `faqmain` | help: FAQ |
| `faqsub` | help: FAQ topic |
| `fep` |  |
| `feppalle` |  |
| `flistmai` | friend list menu |
| `flmes` |  |
| `fp_cat` |  |
| `fp_cat2` | one category of `fp_cat`'s upgrades |
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
| `itemctrl` | item count entry |
| `iteminfo` | item description |
| `itemxinf` | item description, extended |
| `itemyinf` |  |
| `itmsort2` | item sort menu |
| `itmsortw` | storage sort menu |
| `itmstora` |  |
| `iuse` |  |
| `jbpcat` | job points: one job's categories |
| `jobchang` | job change menu |
| `jobcsel` | another name of `jobcselu`, the job list on a Japanese client |
| `jobcselu` | job selection list |
| `joblevel` | job levels (status menu) |
| `k1assign` | key assignments, the `key1_` settings |
| `k2assign` | key assignments, the `key2_` settings |
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
| `mcr1edlo` | another name of `mcr1edit`, in its other layout |
| `mcr1long` |  |
| `mcr1pall` | Ctrl macro bar |
| `mcr2edit` |  |
| `mcr2edlo` | another name of `mcr2edit`, in its other layout |
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
| `merit2ca` | job points: job list |
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
| `mogdoor` | Mog House door: where to go |
| `mogext` |  |
| `mogpost` | post menu: delivery box and send box |
| `money` | gil panel |
| `moneyctr` | gil amount entry |
| `mount` | mount list |
| `mp_abisc` |  |
| `mp_cstm` |  |
| `mp_dead` | Monstrosity KO window |
| `mp_invnt` |  |
| `mp_kncst` |  |
| `mp_mnabi` |  |
| `mp_mncst` |  |
| `mp_mnnm` |  |
| `mp_mntp2` |  |
| `mp_pmode` | action menu while in Monstrosity |
| `mp_stat` |  |
| `msgline` | PlayOnline friend message entry |
| `msglist` | PlayOnline messages |
| `myroom` | Mog House menu |
| `nation1` |  |
| `nation2` |  |
| `nation3` |  |
| `netbar` |  |
| `netstat` |  |
| `netwait` |  |
| `ok` | notice that waits for OK, as when the connection is lost |
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
| `quest00` | the quest log's menu; Unity Information; Ranking Information |
| `quest01` | one list of a `quest00` row |
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
| `rem4li2` | NPC dialogue, over the second chat log |
| `rem4line` | NPC dialogue, over the chat log |
| `resyn` | raise prompt (one of four, by client language) |
| `resynde` | raise prompt (one of four, by client language) |
| `resynfr` | raise prompt (one of four, by client language) |
| `resynus` | raise prompt (one of four, by client language) |
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
| `searchle` | search by rank |
| `searchma` | search menu |
| `searchna` | search by nation |
| `searchra` | search by race |
| `shop` | shop item list |
| `shopbuy` | buy confirm |
| `shopmain` | shop menu |
| `shopsell` | sell confirm |
| `socialme` | system menu (the one Escape opens) |
| `sortyn` | confirm for dropping an item or sorting |
| `splmsg2` |  |
| `spoolmsg` |  |
| `statcom` |  |
| `statcom2` | status menu |
| `statpup` | automaton status |
| `storage` | Mog House storage menu |
| `storage2` | Nomad Moogle storage menu |
| `stringdl` | delivery recipient name entry |
| `subwindo` | sub-target panel under the target window |
| `targetarrow` | the arrows over the target; not a window, see [The target arrows](#the-target-arrows) |
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

## All settings

Every setting, in the Config menu's order, with the game's default. For the
settings named after their page's label, the table also says what the
setting does.

A key's default is a DirectInput key code, with the key at that place on a US
keyboard. On some clients the game keeps other defaults for the keys;
`settings()` gives the running game's.

| name | kind | range or choices | default | page | what it does |
|---|---|---|---|---|---|
| `auto_target` | bool | true or false | true | `conf2win` | locks on to the target as a battle starts |
| `sound_effects_volume` | number | 0 to 100 | 100 | `conf2win` |  |
| `music_volume` | number | 0 to 100 | 100 | `conf2win` |  |
| `gamma_red` | number | 0 to 100 | 50 | `conf2win` |  |
| `gamma_green` | number | 0 to 100 | 50 | `conf2win` |  |
| `gamma_blue` | number | 0 to 100 | 50 | `conf2win` |  |
| `inventory_sort` | bool | true or false | false | `conf2win` | sorts items automatically as they enter the inventory |
| `inventory_type` | choice | type1, type2 | type1 | `conf2win` | type1 sorts items by category; type2 by name, stacking them where it can |
| `filter_say` | number | 0 to 2 | 0 | `cfilter` |  |
| `filter_tell` | number | 0 to 3 | 2 | `cfilter` |  |
| `filter_party` | number | 0 to 1 | 0 | `cfilter` |  |
| `filter_linkshell` | number | 0 to 1 | 0 | `cfilter` |  |
| `filter_linkshell_2` | number | 0 to 1 | 0 | `cfilter` |  |
| `filter_assist_j` | number | 0 to 2 | 0 | `cfilter` |  |
| `filter_assist_e` | number | 0 to 2 | 0 | `cfilter` |  |
| `filter_unity` | number | 0 to 1 | 0 | `cfilter` |  |
| `filter_emotes` | number | 0 to 2 | 0 | `cfilter` |  |
| `filter_shout` | number | 0 to 2 | 0 | `cfilter` |  |
| `filter_yell` | bool | true or false | false | `cfilter` |  |
| `filter_special_actions_started_on_by_you` | bool | true or false | false | `cfilter` |  |
| `filter_special_action_effects_on_by_you` | bool | true or false | false | `cfilter` |  |
| `filter_attacks_by_you` | bool | true or false | false | `cfilter` |  |
| `filter_missed_attacks_by_you` | bool | true or false | false | `cfilter` |  |
| `filter_attacks_you_evade` | bool | true or false | false | `cfilter` |  |
| `filter_damage_you_take` | bool | true or false | false | `cfilter` |  |
| `filter_special_actions_started_on_by_party` | bool | true or false | false | `cfilter` |  |
| `filter_special_action_effects_on_by_party` | bool | true or false | false | `cfilter` |  |
| `filter_attacks_by_party` | bool | true or false | false | `cfilter` |  |
| `filter_missed_attacks_by_party` | bool | true or false | false | `cfilter` |  |
| `filter_attacks_evaded_by_party` | bool | true or false | false | `cfilter` |  |
| `filter_damage_taken_by_party` | bool | true or false | false | `cfilter` |  |
| `filter_special_actions_started_on_by_allies` | bool | true or false | false | `cfilter` |  |
| `filter_special_action_effects_on_by_allies` | bool | true or false | false | `cfilter` |  |
| `filter_attacks_by_allies` | bool | true or false | false | `cfilter` |  |
| `filter_missed_attacks_by_allies` | bool | true or false | false | `cfilter` |  |
| `filter_attacks_evaded_by_allies` | bool | true or false | false | `cfilter` |  |
| `filter_damage_taken_by_allies` | bool | true or false | false | `cfilter` |  |
| `filter_special_actions_started_on_by_foes` | bool | true or false | false | `cfilter` |  |
| `filter_special_action_effects_on_by_foes` | bool | true or false | false | `cfilter` |  |
| `filter_attacks_by_foes` | bool | true or false | false | `cfilter` |  |
| `filter_missed_attacks_by_foes` | bool | true or false | false | `cfilter` |  |
| `filter_attacks_evaded_by_foes` | bool | true or false | false | `cfilter` |  |
| `filter_damage_taken_by_foes` | bool | true or false | false | `cfilter` |  |
| `filter_special_actions_started_on_by_others` | bool | true or false | false | `cfilter` |  |
| `filter_special_action_effects_on_by_others` | bool | true or false | false | `cfilter` |  |
| `filter_attacks_by_others` | bool | true or false | false | `cfilter` |  |
| `filter_missed_attacks_by_others` | bool | true or false | false | `cfilter` |  |
| `filter_attacks_evaded_by_others` | bool | true or false | false | `cfilter` |  |
| `filter_damage_taken_by_others` | bool | true or false | false | `cfilter` |  |
| `filter_special_actions_started_on_by_npcs` | bool | true or false | false | `cfilter` |  |
| `filter_special_action_effects_on_by_npcs` | bool | true or false | false | `cfilter` |  |
| `filter_attacks_by_npcs` | bool | true or false | false | `cfilter` |  |
| `filter_missed_attacks_by_npcs` | bool | true or false | false | `cfilter` |  |
| `filter_others_synthesis_and_fishing_results` | bool | true or false | false | `cfilter` |  |
| `filter_lot_results` | bool | true or false | false | `cfilter` |  |
| `filter_campaign_related_data` | bool | true or false | false | `cfilter` |  |
| `filter_tell_messages_deemed_spam` | bool | true or false | true | `cfilter` |  |
| `filter_shout_yell_messages_deemed_spam` | bool | true or false | true | `cfilter` |  |
| `filter_job_specific_emote` | bool | true or false | false | `cfilter` |  |
| `filter_messages_from_alter_egos` | number | 0 to 2 | 0 | `cfilter` |  |
| `filter_system_level` | number | 0 to 3 | 0 | `cfilter` |  |
| `color_say` | color | 0 to 255 each | 128, 128, 128 | `conftxtc` |  |
| `color_tell` | color | 0 to 255 each | 160, 64, 160 | `conftxtc` |  |
| `color_party` | color | 0 to 255 each | 32, 192, 160 | `conftxtc` |  |
| `color_linkshell` | color | 0 to 255 each | 80, 255, 96 | `conftxtc` |  |
| `color_linkshell_2` | color | 0 to 255 each | 0, 204, 0 | `conftxtc` |  |
| `color_assist_j` | color | 0 to 255 each | 0, 80, 255 | `conftxtc` |  |
| `color_assist_e` | color | 0 to 255 each | 0, 112, 255 | `conftxtc` |  |
| `color_unity` | color | 0 to 255 each | 255, 175, 63 | `conftxtc` |  |
| `color_emotes` | color | 0 to 255 each | 96, 80, 160 | `conftxtc` |  |
| `color_message` | color | 0 to 255 each | 160, 208, 208 | `conftxtc` |  |
| `color_npc_conversations` | color | 0 to 255 each | 128, 128, 128 | `conftxtc` |  |
| `color_shout` | color | 0 to 255 each | 160, 80, 64 | `conftxtc` |  |
| `color_yell` | color | 0 to 255 each | 160, 64, 48 | `conftxtc` |  |
| `color_hp_mp_you_recover` | color | 0 to 255 each | 96, 144, 192 | `conftxtc` |  |
| `color_hp_mp_you_lose` | color | 0 to 255 each | 160, 64, 64 | `conftxtc` |  |
| `color_beneficial_effects_you_are_granted` | color | 0 to 255 each | 128, 128, 128 | `conftxtc` |  |
| `color_detrimental_effects_you_receive` | color | 0 to 255 each | 128, 128, 128 | `conftxtc` |  |
| `color_effects_you_resist` | color | 0 to 255 each | 128, 128, 80 | `conftxtc` |  |
| `color_actions_you_evade` | color | 0 to 255 each | 128, 128, 128 | `conftxtc` |  |
| `color_hp_mp_others_recover` | color | 0 to 255 each | 144, 192, 240 | `conftxtc` |  |
| `color_hp_mp_others_lose` | color | 0 to 255 each | 192, 128, 128 | `conftxtc` |  |
| `color_beneficial_effects_others_are_granted` | color | 0 to 255 each | 128, 128, 128 | `conftxtc` |  |
| `color_detrimental_effects_others_receive` | color | 0 to 255 each | 128, 128, 128 | `conftxtc` |  |
| `color_effects_others_resist` | color | 0 to 255 each | 160, 128, 64 | `conftxtc` |  |
| `color_actions_others_evade` | color | 0 to 255 each | 112, 112, 112 | `conftxtc` |  |
| `color_standard_battle_messages` | color | 0 to 255 each | 128, 128, 16 | `conftxtc` |  |
| `color_calls_for_help` | color | 0 to 255 each | 192, 96, 208 | `conftxtc` |  |
| `color_basic_system_messages` | color | 0 to 255 each | 192, 192, 80 | `conftxtc` |  |
| `log_multi_window` | choice | vertical, horizontal, off | off | `conf5win` | vertical splits the log window into upper and lower ones, horizontal into left and right |
| `timestamp` | choice | minutes, seconds, off | off | `conf5win` | time stamps in the log window, down to the minute or the second |
| `window_type` | number | 0 to 7 | 0 | `conf5win` | the windows' pattern, types one to eight as 0 to 7 |
| `window_effect` | bool | true or false | true | `conf5win` | shows the windows' opening and closing effects |
| `log_window1_reactive_sizing` | bool | true or false | true | `conf5w1` | sizes the log window to the amount of text; off, it keeps a set size |
| `log_window1_max_lines` | number | 4 to 20 | 10 | `conf5w1` | the most lines the log window shows |
| `log_window1_min_lines` | number | 0 to 20 | 0 | `conf5w1` | the fewest lines the log window shows |
| `log_window1_width` | number | 0 to 100 | 100 | `conf5w1` | the log window's column width |
| `log_window1_resize_time` | number | 3 to 30 | 5 | `conf5w1` | the time between the log window's size adjustments |
| `log_window2_reactive_sizing` | bool | true or false | true | `conf5w2` | sizes the log window to the amount of text; off, it keeps a set size |
| `log_window2_max_lines` | number | 4 to 20 | 10 | `conf5w2` | the most lines the log window shows |
| `log_window2_min_lines` | number | 0 to 20 | 0 | `conf5w2` | the fewest lines the log window shows |
| `log_window2_width` | number | -1 to 100 | -1 | `conf5w2` | the log window's column width; -1 copies log window 1's value, once |
| `log_window2_resize_time` | number | -1 to 30 | -1 | `conf5w2` | the time between the log window's size adjustments; -1 copies log window 1's value, once |
| `damage_display` | choice | log, both, screen | both | `conf3win` | damage in the log window, on the screen and in the log, or on the screen only |
| `character_information` | choice | show, hide | none, the server keeps it | `conf3win` | hide keeps your character's data out of party and Friend List searches |
| `shadows` | choice | high, normal, off | normal | `conf3win` | detailed, simple or no character shadows |
| `weather_effects` | bool | true or false | true | `conf3win` | shows particle effects for weather and the like |
| `character_models_displayed` | number | 25 to 51 | 50 | `conf3win` | the most characters shown on the screen |
| `icon_type` | choice | type1, type2 | type2 | `conf3win` | the icons' pattern |
| `pc_armor_display` | choice | normal, static | normal | `conf3win` | static shows other players' armor statically, for performance |
| `clipping_plane` | number | 0 to 25 | 25 | `conf6win` | the distance of the 3D view's back clipping plane |
| `footstep_effects` | bool | true or false | true | `conf6win` | shows footprints, ripples and the like |
| `animation_frame_rate` | number | 0 to 3 | 3 | `conf6win` | limits the characters' animation rate to keep the frame rate |
| `keyboard_size` | choice | full, compact1, compact2 | compact2 | `conf6win` | the input method for a full keyboard, or for compact ones without a numeric keypad |
| `background_aspect_ratio` | number | 0 to 5 | 0 | `conf6win` | fits the background to the monitor's aspect ratio |
| `macro_palette_size` | choice | full, compact | full | `conf6win` | the macro palette's size |
| `macro_palette_position` | choice | dynamic, static | dynamic | `conf6win` | dynamic shifts the macro palette right during battle; static keeps it right |
| `key1_move_up` | key | 0 to 255 | 17 (W) | `k1assign` |  |
| `key1_move_down` | key | 0 to 255 | 31 (S) | `k1assign` |  |
| `key1_move_left` | key | 0 to 255 | 30 (A) | `k1assign` |  |
| `key1_move_right` | key | 0 to 255 | 32 (D) | `k1assign` |  |
| `key1_menu_cursor_camera_up` | key | 0 to 255 | 23 (I) | `k1assign` |  |
| `key1_menu_cursor_camera_down` | key | 0 to 255 | 37 (K) | `k1assign` |  |
| `key1_menu_cursor_camera_left` | key | 0 to 255 | 36 (J) | `k1assign` |  |
| `key1_menu_cursor_camera_right` | key | 0 to 255 | 38 (L) | `k1assign` |  |
| `key1_move_camera_up` | key | 0 to 255 | 0 (none) | `k1assign` |  |
| `key1_move_camera_down` | key | 0 to 255 | 0 (none) | `k1assign` |  |
| `key1_move_camera_left` | key | 0 to 255 | 0 (none) | `k1assign` |  |
| `key1_move_camera_right` | key | 0 to 255 | 0 (none) | `k1assign` |  |
| `key1_zoom_in` | key | 0 to 255 | 52 (.) | `k1assign` |  |
| `key1_zoom_out` | key | 0 to 255 | 51 (,) | `k1assign` |  |
| `key1_menu_target_cursor_up` | key | 0 to 255 | 0 (none) | `k1assign` |  |
| `key1_menu_target_cursor_down` | key | 0 to 255 | 0 (none) | `k1assign` |  |
| `key1_menu_target_cursor_left` | key | 0 to 255 | 0 (none) | `k1assign` |  |
| `key1_menu_target_cursor_right` | key | 0 to 255 | 0 (none) | `k1assign` |  |
| `key1_confirm` | key | 0 to 255 | 21 (Y) | `k1assign` |  |
| `key1_cancel` | key | 0 to 255 | 49 (N) | `k1assign` |  |
| `key1_change_view` | key | 0 to 255 | 47 (V) | `k1assign` |  |
| `key1_autorun` | key | 0 to 255 | 19 (R) | `k1assign` |  |
| `key1_heal` | key | 0 to 255 | 35 (H) | `k1assign` |  |
| `key1_select_window` | key | 0 to 255 | 33 (F) | `k1assign` |  |
| `key1_open_main_menu` | key | 0 to 255 | 12 (-) | `k1assign` |  |
| `key1_toggle_walk_run` | key | 0 to 255 | 44 (Z) | `k1assign` |  |
| `key1_hide_show_window` | key | 0 to 255 | 27 (]) | `k1assign` |  |
| `key1_take_screenshot` | key | 0 to 255 | 26 ([) | `k1assign` |  |
| `key1_turn_right` | key | 0 to 255 | 18 (E) | `k1assign` |  |
| `key1_turn_left` | key | 0 to 255 | 16 (Q) | `k1assign` |  |
| `key2_attack` | key | 0 to 255 | 30 (A) | `k2assign` |  |
| `key2_items` | key | 0 to 255 | 23 (I) | `k2assign` |  |
| `key2_magic` | key | 0 to 255 | 50 (M) | `k2assign` |  |
| `key2_abilities` | key | 0 to 255 | 36 (J) | `k2assign` |  |
| `key2_weapon_skills` | key | 0 to 255 | 17 (W) | `k2assign` |  |
| `key2_pet_commands` | key | 0 to 255 | 48 (B) | `k2assign` |  |
| `key2_confirm` | key | 0 to 255 | 21 (Y) | `k2assign` |  |
| `key2_cancel` | key | 0 to 255 | 49 (N) | `k2assign` |  |
| `key2_open_main_menu` | key | 0 to 255 | 12 (-) | `k2assign` |  |
| `key2_select_window` | key | 0 to 255 | 33 (F) | `k2assign` |  |
| `key2_hide_show_window` | key | 0 to 255 | 27 (]) | `k2assign` |  |
| `key2_take_screenshot` | key | 0 to 255 | 26 ([) | `k2assign` |  |
| `key2_toggle_walk_run` | key | 0 to 255 | 44 (Z) | `k2assign` |  |
| `key2_zoom_in` | key | 0 to 255 | 52 (.) | `k2assign` |  |
| `key2_zoom_out` | key | 0 to 255 | 51 (,) | `k2assign` |  |
| `key2_ranged_attack` | key | 0 to 255 | 0 (none) | `k2assign` |  |
| `key2_call_for_help` | key | 0 to 255 | 0 (none) | `k2assign` |  |
| `key2_equipment` | key | 0 to 255 | 18 (E) | `k2assign` |  |
| `key2_change_view` | key | 0 to 255 | 47 (V) | `k2assign` |  |
| `key2_heal` | key | 0 to 255 | 35 (H) | `k2assign` |  |
| `key2_check` | key | 0 to 255 | 46 (C) | `k2assign` |  |
| `key2_chat_say` | key | 0 to 255 | 31 (S) | `k2assign` |  |
| `key2_chat_party` | key | 0 to 255 | 25 (P) | `k2assign` |  |
| `key2_chat_tell` | key | 0 to 255 | 20 (T) | `k2assign` |  |
| `key2_chat_linkshell` | key | 0 to 255 | 38 (L) | `k2assign` |  |
| `key2_chat_reply_to_tell` | key | 0 to 255 | 19 (R) | `k2assign` |  |
| `key2_chat_unity` | key | 0 to 255 | 22 (U) | `k2assign` |  |
| `key2_chat_linkshell2` | key | 0 to 255 | 45 (X) | `k2assign` |  |
| `key2_chat_assiste_or_assistj` | key | 0 to 255 | 16 (Q) | `k2assign` |  |
| `effect_filter_all_effects_during_battle` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_you_to_you` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_you_to_monster` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_you_to_party` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_you_to_non_party_pc` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_monster_to_you` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_monster_to_monster` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_monster_to_party` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_monster_to_non_party_pc` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_party_to_you` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_party_to_monster` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_party_to_party` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_party_to_non_party_pc` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_non_party_pc_to_you` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_non_party_pc_to_monster` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_non_party_pc_to_party` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_non_party_pc_to_non_party_pc` | bool | true or false | false | `fxfilter` |  |
| `effect_filter_screen_shaking` | bool | true or false | false | `fxfilter` |  |
| `mouse_control` | choice | type1, type2 | type1 | `conf7` | the mouse's input method |
| `screen_edge_panning` | bool | true or false | false | `conf7` | turns the camera as the mouse pointer reaches the screen's edge |
| `camera_view` | choice | normal, chase_cam | normal | `conf7` | chase_cam keeps the third-person camera behind your character |
| `third_person_y_axis` | choice | normal, inverted | inverted | `conf7` | inverts the third-person camera's Y axis |
| `third_person_x_axis` | choice | normal, inverted | inverted | `conf7` | inverts the third-person camera's X axis |
| `first_person_y_axis` | choice | normal, inverted | normal | `conf7` | inverts the first-person camera's Y axis |
| `first_person_x_axis` | choice | normal, inverted | normal | `conf7` | inverts the first-person camera's X axis |
| `chat_language_filter` | bool | true or false | true | `conf4` | filters out words that could be read as offensive |
| `auto_disconnect` | number | -1 to 60 | -1 | `conf4` | disconnects from the server after this many idle minutes; -1 is off |
| `log1_say` | bool | true or false | false | `conf11s` |  |
| `log1_tell` | bool | true or false | false | `conf11s` |  |
| `log1_party` | bool | true or false | false | `conf11s` |  |
| `log1_linkshell` | bool | true or false | false | `conf11s` |  |
| `log1_linkshell_2` | bool | true or false | false | `conf11s` |  |
| `log1_assist_j` | bool | true or false | false | `conf11s` |  |
| `log1_assist_e` | bool | true or false | false | `conf11s` |  |
| `log1_unity` | bool | true or false | false | `conf11s` |  |
| `log1_emotes` | bool | true or false | false | `conf11s` |  |
| `log1_message` | bool | true or false | false | `conf11s` |  |
| `log1_npc_conversations` | bool | true or false | false | `conf11s` |  |
| `log1_shout` | bool | true or false | false | `conf11s` |  |
| `log1_yell` | bool | true or false | false | `conf11s` |  |
| `log1_hp_mp_you_recover` | bool | true or false | true | `conf11s` |  |
| `log1_hp_mp_you_lose` | bool | true or false | true | `conf11s` |  |
| `log1_beneficial_effects_you_are_granted` | bool | true or false | true | `conf11s` |  |
| `log1_detrimental_effects_you_receive` | bool | true or false | true | `conf11s` |  |
| `log1_effects_you_resist` | bool | true or false | true | `conf11s` |  |
| `log1_actions_you_evade` | bool | true or false | true | `conf11s` |  |
| `log1_hp_mp_others_recover` | bool | true or false | true | `conf11s` |  |
| `log1_hp_mp_others_lose` | bool | true or false | true | `conf11s` |  |
| `log1_beneficial_effects_others_are_granted` | bool | true or false | true | `conf11s` |  |
| `log1_detrimental_effects_others_receive` | bool | true or false | true | `conf11s` |  |
| `log1_effects_others_resist` | bool | true or false | true | `conf11s` |  |
| `log1_actions_others_evade` | bool | true or false | true | `conf11s` |  |
| `log1_battle_messages` | bool | true or false | true | `conf11s` |  |
| `log1_calls_for_help` | bool | true or false | true | `conf11s` |  |
| `log1_basic_system_messages` | bool | true or false | true | `conf11s` |  |
| `log2_say` | bool | true or false | true | `conf11s` |  |
| `log2_tell` | bool | true or false | true | `conf11s` |  |
| `log2_party` | bool | true or false | true | `conf11s` |  |
| `log2_linkshell` | bool | true or false | true | `conf11s` |  |
| `log2_linkshell_2` | bool | true or false | true | `conf11s` |  |
| `log2_assist_j` | bool | true or false | true | `conf11s` |  |
| `log2_assist_e` | bool | true or false | true | `conf11s` |  |
| `log2_unity` | bool | true or false | true | `conf11s` |  |
| `log2_emotes` | bool | true or false | true | `conf11s` |  |
| `log2_message` | bool | true or false | true | `conf11s` |  |
| `log2_npc_conversations` | bool | true or false | true | `conf11s` |  |
| `log2_shout` | bool | true or false | true | `conf11s` |  |
| `log2_yell` | bool | true or false | true | `conf11s` |  |
| `log2_hp_mp_you_recover` | bool | true or false | false | `conf11s` |  |
| `log2_hp_mp_you_lose` | bool | true or false | false | `conf11s` |  |
| `log2_beneficial_effects_you_are_granted` | bool | true or false | false | `conf11s` |  |
| `log2_detrimental_effects_you_receive` | bool | true or false | false | `conf11s` |  |
| `log2_effects_you_resist` | bool | true or false | false | `conf11s` |  |
| `log2_actions_you_evade` | bool | true or false | false | `conf11s` |  |
| `log2_hp_mp_others_recover` | bool | true or false | false | `conf11s` |  |
| `log2_hp_mp_others_lose` | bool | true or false | false | `conf11s` |  |
| `log2_beneficial_effects_others_are_granted` | bool | true or false | false | `conf11s` |  |
| `log2_detrimental_effects_others_receive` | bool | true or false | false | `conf11s` |  |
| `log2_effects_others_resist` | bool | true or false | false | `conf11s` |  |
| `log2_actions_others_evade` | bool | true or false | false | `conf11s` |  |
| `log2_battle_messages` | bool | true or false | false | `conf11s` |  |
| `log2_calls_for_help` | bool | true or false | false | `conf11s` |  |
| `log2_basic_system_messages` | bool | true or false | false | `conf11s` |  |
| `weapon_effect` | bool | true or false | true | `conf12wi` | shows weapon effects |
| `style_lock` | choice | normal, static | normal | `conf12wi` | static fixes the look of your own equipment; the game allows one change every 9 seconds |
| `area_display` | bool | true or false | true | `conf12wi` | outlines the area of effect while a spell or ability picks its target |
| `target_expressions` | bool | true or false | true | `conf12wi` | shows an effect on what the target cursors point at |
| `party_icon_display` | bool | true or false | true | `conf12wi` | shows the party members' status icons |
| `status_icon_timer_display` | bool | true or false | true | `conf12wi` | shows the status icons' timers |
| `furniture_camera_collision` | bool | true or false | true | `conf12wi` | the camera collides with Mog House furniture |
| `software_keyboard` | choice | off, full, compact | off | `conf13wi` | the standard or the compact software keyboard, or none |
| `term_filter` | choice | on, display_name_only, off | off | `conf13wi` | hides chat messages with the terms your filter settings name |

## License

0BSD. See [LICENSE](LICENSE).
