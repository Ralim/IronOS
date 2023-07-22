# GUI Rendering

The GUI aims to be somewhat similar to immediate mode rendering, where the screen is re-rendered each sweep.
This is due to a few aims:

1. Functions should try and contain their state to the context struct (helps keep state usage flatter)
2. Allows external events to change the state
3. Means state can be read/write over BLE or other external control interfaces

## Transitions

When changing the view to a new view it can be preferable to transition using an animation.
The tooling provides for left, right and down animations at this point.
The use of these gives a notion of "direction" when navigating the menu.

```
                          ┌───────────┐
                          │ Debug Menu│
                          └─────┬─────┘
                                │
                                │
                                │
┌──────────────┐           ┌────┴─────┐           ┌──────────────────┐           ┌─────────────────┐
│Soldering Mode│           │          │           │                  │           │                 │
│      OR      ├───────────┤Home Menu ├───────────┤Settings Main Menu├───────────┤Settings sub menu│
│Reflow    Mode│           │          │           │                  │           │                 │
└──────────────┘           └──────────┘           └──────────────────┘           └─────────┬───────┘
                                                                                           │
                                                                                 ┌─────────┴───────┐
                                                                                 │                 │
                                                                                 │Settings sub menu│
                                                                                 │                 │
                                                                                 └─────────────────┘
```

The downside of supporting transitions is that for these to work, the code should render the screen _first_ then return the new state.
This ensures there is a good working copy in the buffer before the transition changes the view.

The code that handles the dispatch will run a new render pass again to get the new buffer contents and then transition between the two for you.
At the moment scrolling "Up" isn't implemented but the enumeration is there so that its implementation can follow.
