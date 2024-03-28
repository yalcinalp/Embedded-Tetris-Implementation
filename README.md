# Assembly-Tetris
Tetris Game Implementation in Assembly for the LED board of PICKit (Under Construction)

# Device:  

![image](https://github.com/yalcinalp/Assembly-Tetris/assets/95969634/d700872b-76cb-4cf2-bafd-2614f9b04cf8)

# Display:  

![image](https://github.com/yalcinalp/Assembly-Tetris/assets/95969634/947ae16f-bbc1-4a15-b3b9-ed6c7a3032fc)

# Shapes of the Tetris game:  

![image](https://github.com/yalcinalp/Assembly-Tetris/assets/95969634/6b4746be-1d87-40bb-8ab5-2b9aebadbb77)

# Rotation order:  

![image](https://github.com/yalcinalp/Assembly-Tetris/assets/95969634/c6cd3dcd-bc25-4510-903e-079f6244b279)

# Example Runtime: 

![image](https://github.com/yalcinalp/Assembly-Tetris/assets/95969634/433364ca-5b72-4e76-98c0-9af641632154)

# Game Loop:  
The game system will provide a Tetris piece to the user.  

• Until the user submits this piece, this piece will be “alive”.  

• The user can interact with this piece using input buttons.  

• This piece will gradually fall every 2 second. This means the live piece will vertically drop one square (LED) every 2 seconds.  

• This piece will be blinking every 250ms. The first 250ms the piece will be on. Next 250ms, it will be off, and so on.  

• User may rotate this piece at any time (PORTB5). The piece will be rotated in clock- wise order.  

 
