# Visual Game

* main: user interface, only about game visual
    1. while game status change, update view
    2. while complete game, move into finish menu 
* GameController:
	1. generate game visual and img process
	2. detect color position then return view unit stop
	3. update game status
	4. check the game status, if finish move into finish menu
	5. restart game or end game
* GameParameter: game variable and initilize