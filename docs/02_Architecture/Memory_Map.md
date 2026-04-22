>"""64 MB of RAM is more than the necessary for this application", we don't see it like that, 64 is the ceiling, it isn't obligatory nor limited, the recommended value is actually 28 MB, but we'll never tell you what to do."
## Why 28 MB?
- The idea is to give enough RAM for 2 720p screens, [more info](../03_Hardware/Expansion_Connector); The OS, and the game to run at the same time.
	- The screens: For 720p(1280x720), RGBA(4 bytes), double buffering, a single screen uses 7.03 MB of RAM, for two of them 14.06 MB
	- The OS: Our objective is to make it run as lean as possible, realistically saying, just the OS logic we expect to run, when focused at around 1 - 2 MB, and less than 1 MB when in background.
	- Leaving around 12 MB of RAM for the game itself.
But you are free to use how much RAM your heart desires, we recommend this value as it's enough for basic 3D rendering, even less if you make a 2D game, as it leaves the player's experience at a enjoyable state, it's not using everything, so it's a smooth experience.
## Why 64 and not 32 MB then?
>"Good question, if you actually thought of that, but the reality of the market doesn't landed us the ideal SoC, this was actually our objective with the SoC we were going to choose, the math works, but in practice, the existing SoC chips on the market only operate from 64 MB and above, because, starting in DDR3, the actual chips of memory were made to be of more and more capacity, now starting in 64MB, as the PC market required more RAM."
