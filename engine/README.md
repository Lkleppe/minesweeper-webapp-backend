### Engine

There will be an instance of an engine for each game being played by users. Each server need not know about how the engine handles everything, it just needs an interface for "clicking" on tiles. E.g., the server sends commands to the engine, such as flagging a tile at position 0, 0, or clicking a tile at 1,2, etc. The engine then sends back information about what happened when the tile was clicked/flagged.

### Board Initialization

Board initialization is done *after* the first tile is clicked. n bomb positions are randomly selected via rng, and the first tile clicked cannot be a bomb.

### Clicking a Tile

Clicking a tile should return a message saying what happened, and if it wasn't a bomb, then information about the number of bombs adjacent to the tile should be revealed. Clicking a tile should also cause a breadth-first-search algorithm to run that iteratively reveals tiles in the surrounding area that are adjacent to zero bombs. It should reveal all tiles adjacent to tiles adjacent to zero bombs, but stop once reaching those.