#include <stdio.h> //printf
#include <termios.h> //init low level terminal
#include <stdlib.h> //malloc
#include <unistd.h> //usleep
#include <stdbool.h> //bool
#include <sys/ioctl.h> //get terminal window size
#include <pthread.h> // threads
#include <signal.h> // safe stoping on Ctrl+C
#include <time.h> // for srand(time(NULL))

#define SetMousePos(x,y) printf("\033[%d;%dH", (y), (x))
#define Uint8 unsigned char
#define Uint16 unsigned short int
 
char pressedKey = 0;

char OppositeDirection(char direction)
{
    switch (direction)
    {
        case 'L':
        return 'R';
        case 'R':
        return 'L';
        case 'U':
        return 'D';
        case 'D':
        return 'U';
    }
    return ' ';
}

void game(Uint16 width, Uint16 height)
{

    if (width * height > 10000)
    {
        return;
    }


//--------------OnStart-----------------
    Uint16 Snake_length = 5;
    Uint16* game_field = malloc(sizeof(Uint16) * (width * height));
    char currentDirection = 'R';
    char buttonQue[3] = {' ', ' ', ' '};
    bool SaveFrame = false;

    char Frame = 0;


    Snake_length = 5;


	for (Uint16 n = 5; n < width * height; n++) 
    {
		game_field[n] = 0;
	}


	for (Uint16 n = 0; n < 5; n++) 
    {
		game_field[n] = n + 1;
	}
	srand(time(NULL));
	game_field[(rand() % ((width * height) - 8)) + 5] = 10000;

	Uint16 x = 5;
  Uint16 y = 0;
//--------------------------------------
    char newDirection = 0;

    while (pressedKey != 'q'){

        usleep(150000);

        if (pressedKey == 'L' || pressedKey == 'R' || pressedKey == 'U' || pressedKey == 'D')
        {
            if (newDirection != pressedKey)
            {

                newDirection = pressedKey;

                for (Uint8 n = 0; n < 3; n++)
                {
                    if (buttonQue[n] == ' ' && newDirection != 0)
                    {
                        buttonQue[n] = newDirection;
                        break;
                    }
                }
            }
        }

        Frame++;
        if (Frame % 1 == 0) 
        {


            Frame = 0;
            for (int n = 0; n < width * height; n++) 
            {

                if (game_field[n] == Snake_length) 
                {

                    if (buttonQue[0] != ' ')
                    {

                        if (buttonQue[0] != OppositeDirection(currentDirection))
                            currentDirection = buttonQue[0];

                        buttonQue[0] = buttonQue[1];
                        buttonQue[1] = buttonQue[2];
                        buttonQue[2] = ' ';
                    }

                    x = n % width;
                    y = n / width;

                    switch (currentDirection)
                    {
                    case 'R':
                        x++;
                        break;
                    case 'L':
                        x--;
                        break;
                    case 'U':
                        y--;
                        break;
                    case 'D':
                        y++;
                        break;
                    default:
                        break;
                    }
                    if (x > 16000) 
                    {
                        x = width -1 ;
                    }
                    if (y > 16000) 
                    {
                        y = height - 1;
                    }
                    y %= height;
                    x %= width;



                    if (game_field[y * width + x] == 10000) 
                    {
                        Snake_length++;
                        game_field[y * width + x] = Snake_length;
                        for (int n = 0; n < width * height; n++) 
                        {
                            if (game_field[n] != 0) {
                                game_field[n]++;
                            }
                        }
                        srand(x + y + Snake_length + currentDirection + time(NULL));
                        Uint16 new_apple;

                        const Uint16 availableSpaces = (width * height) - Snake_length;




                        new_apple = rand() % (availableSpaces -1);

                        new_apple++;

                        Uint16 counter = 0;
                        for (Uint16 n = 0; n < width * height; n++)
                        {
                            if (game_field[n] == 0)
                            {
                                counter++;
                                if (counter == new_apple)
                                    new_apple = n;
                            }
                        }

                        game_field[new_apple] = 10000;
                        break;
                    }

                    if (game_field[y * width + x] != 0 && game_field[y * width + x] != 1) 
                    {
                        for (Uint16 n = 0; n < width * height; n++)
                            if (game_field[n] != 0)
                                if (game_field[n] != 10000)
                                    game_field[n] += 1;

                        if (SaveFrame)
                            goto out;
                        SaveFrame = true;


                        usleep(200000);
                        break;

                    }
                    else 
                    {
                        game_field[y * width + x] = Snake_length + 1;
                        SaveFrame = false;
                    }

                }


            }
            for (Uint16 n = 0; n < width * height; n++)
                if (game_field[n] != 0)
                    if (game_field[n] != 10000)
                        game_field[n] -= 1;

            for (Uint8 n = 0; n < 3; n++)
            {
                if (buttonQue[n] == ' ' && newDirection != 0)
                {
                    buttonQue[n] = newDirection;
                    break;
                }
            }
        }
        //render
        struct winsize w;

        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        system("clear");

        if (w.ws_row != height || w.ws_col != width)
        {

            printf("YOU CAN'T CHANGE THE WINDOW SIZE\n\nPRESS Q TO EXIT...\n");

            goto out;
        }
        

        for (Uint16 index = 0; index < width * height; index++)
        {
            if (game_field[index] != 0)
            {
                if (game_field[index] == 10000)
                {
                    printf("\033[1;41m \033[0m");
                }
                else
                {
                    printf("\033[1;47m \033[0m");
                }
            }
            else
            {
                printf(" ");
            }
        }

        fflush(stdout);

    }
    out:

    pressedKey = 'q';

    free(game_field);

    system("clear");

    printf("YOU LOSE!\n\nPRESS Q TO EXIT...\n");

    return;
}

void showCursor(bool show) 
{ 
//https://stackoverflow.com/questions/2649733/hide-cursor-on-remote-terminal#9604357
#define CSI "\e["
  if (show) {
    fputs(CSI "?25h", stdout);
  }
  else {
    fputs(CSI "?25l", stdout);
  }
#undef CSI
}


void SigintHandler(int sig_num)
{
    signal(SIGINT, SigintHandler);
    pressedKey = 'q';

    //showCursor(true);
}

void* KeyUpdateLoop(void* glob_var_key_void)
{
    
    do 
    {
        char TMPpressedKey = getchar();


        if (TMPpressedKey == 27)
        {
            TMPpressedKey = getchar();
            if (TMPpressedKey == 91)
            {
                TMPpressedKey = getchar();
                switch (TMPpressedKey)
                {
                case 65: //up
                    TMPpressedKey = 'U';
                    break;
                case 66: //down
                    TMPpressedKey = 'D';
                    break;
                case 68: //left
                    TMPpressedKey = 'L';
                    break;
                case 67: //rigth 
                    TMPpressedKey = 'R';
                    break; 
                
                default:
                    break;
                }
            }
        }

        

        //printf("%i\n", (int)TMPpressedKey);

        pressedKey = TMPpressedKey;  
    } while(pressedKey != 'q');

    

    pthread_exit(NULL);

    return 0;
}

int main()
{

    //signal(SIGINT, SigintHandler);

    struct termios old_tio, new_tio;
    unsigned char c;

    /* get the terminal settings for stdin */
    tcgetattr(STDIN_FILENO,&old_tio);

    /* we want to keep the old setting to restore them a the end */
    new_tio=old_tio;

    /* disable canonical mode (buffered i/o) and local echo */
    new_tio.c_lflag &=(~ICANON & ~ECHO);
    

    /* set the new settings immediately */
    tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);


    /* restore the former settings */

  

    struct winsize w;

    showCursor(false);

    fflush(stdout);

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    //thrd_t KeyCheckingThread; 

    pthread_t KeyCheckingThread;

    //thrd_create(&KeyCheckingThread, KeyUpdateLoop, NULL);

    pthread_create(&KeyCheckingThread, NULL, KeyUpdateLoop, NULL);

    
    usleep(10000);

    game(w.ws_col, w.ws_row);
    
    pthread_join(KeyCheckingThread, NULL);

    showCursor(true);
    
    tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);

    system("reset");

    return 0;
}
