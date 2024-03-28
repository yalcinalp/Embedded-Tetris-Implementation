# Assembly-Tetris
Tetris Game Implementation in Assembly for the LED board of PICKit

Device:  

Display:  

Shapes of the Tetris game:  

Rotation order:  

Example Runtime: 

Game Loop:  
The game system will provide a Tetris piece to the user.  

• Until the user submits this piece, this piece will be “alive”.  

• The user can interact with this piece using input buttons.  

• This piece will gradually fall every 2 second. This means the live piece will vertically drop one square (LED) every 2 seconds.  

• This piece will be blinking every 250ms. The first 250ms the piece will be on. Next 250ms, it will be off, and so on.  

• User may rotate this piece at any time (PORTB5). The piece will be rotated in clock- wise order.  

 
