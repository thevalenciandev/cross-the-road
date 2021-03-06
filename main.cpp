#include "stdlib.h" // for rand
#include <deque>
#include <vector>
#include <ncurses.h>
#include <time.h>

using namespace std;

class Player
{
public:
    int x, y;
    Player(int width)
    {
        x = width / 2; // initialise Player in the middle of the "map"
        y = 0;
    }
};

class Lane
{
private:
    deque<bool> cars; // Series of "dots": if true there is a car in there, else nothing is there
    bool moveRight;

public:
    Lane(int width)
    {
        for (int i = 0; i < width; i++)
        {
            cars.push_front(false); // no cars in the lane, to start with.
        }
        moveRight = rand() % 2;
    }
    void Move()
    {
        if (moveRight)
        {
            if (rand() % 10 == 1) // randomly create cars.
                cars.push_front(true);
            else
                cars.push_front(false);

            cars.pop_back(); // remove last car, so the lane stays constant in size.
        }
        else
        {
            if (rand() % 10 == 1) // randomly create cars.
                cars.push_back(true);
            else
                cars.push_back(false);

            cars.pop_front(); // remove front car, so the lane stays constant in size.
        }
    }
    bool CarExists(int pos) { return cars[pos]; }
    void ChangeDirection() { moveRight = !moveRight; }
};

class Game
{
private:
    bool quit = false;
    int numberOfLanes;
    int width;
    int score = 0;
    Player *player;
    vector<Lane *> map;

    void InitGraphics()
    {
        // Using ncurses lib.
        initscr();            // init curses mode (allocs mem for present window, stdscr)
        noecho();             // inputs by user will not be displayed on screen
        cbreak();             // read character without having to press ENTER (disable line buffering)
        keypad(stdscr, TRUE); // enable arrow keys
        scrollok(stdscr, TRUE);
        nodelay(stdscr, TRUE); // non-blocking getchar for user input
        start_color();
        init_pair(1, COLOR_CYAN, COLOR_BLACK); // define character's color (foreground, background)
    }
    void TearDownGraphics()
    {
        endwin(); // end curses mode. Failure to do so will result in weird terminal behaviour.
    }
    void Draw()
    {
        move(0, 0); // move cursor to beginning
        for (int i = 0; i < numberOfLanes; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if ((i == 0 || i == numberOfLanes - 1) && (j == 0 || j == width - 1))
                    addch('|');
                if (map[i]->CarExists(j) && i != 0 && i != numberOfLanes - 1)
                    // print car, but ignore first and last lanes so the player
                    // can start and finish on an empty lane.
                    addch('~');
                else if (player->x == j && player->y == i)
                {
                    attron(COLOR_PAIR(1));
                    addch('@'); // print player
                    attroff(COLOR_PAIR(1));
                }
                else
                    addch(' ');
            }
            addch('\n');
        }
        attron(A_BOLD);
        printw("Score: %d", score);
        attroff(A_BOLD);
        refresh(); // Actual print on the screen.
        napms(25);
    }
    void Input()
    {
        int current = getch();
        if (current == KEY_DOWN || current == 'j')
            player->y++;
        if (current == KEY_UP || current == 'k')
            player->y--;
        if (current == KEY_LEFT || current == 'h')
            player->x--;
        if (current == KEY_RIGHT || current == 'l')
            player->x++;
        if (current == 'q')
            quit = true;
    }
    void Logic()
    {
        // Run this only for the lanes with cars.
        for (int i = 1; i < numberOfLanes - 1; i++)
        {
            if (rand() % 10 == 1)
                map[i]->Move(); // move lane.
            if (player->y == i && map[i]->CarExists(player->x))
                quit = true; // if there is a hit, stop.
        }
        if (player->y == numberOfLanes - 1)
        {
            score++;
            player->y = 0; // return to the top
            map[rand() % numberOfLanes]->ChangeDirection();
        }
    }
    void ShowGameOver()
    {
        addstr("\nGAME OVER. Press ENTER to quit.");
        refresh();
        timeout(-1); // block indefinitely until user presses key.
        int current;
        do
        {
            current = getch();
        } while (current != '\n'); // KEY_ENTER unreliable...
    }

public:
    Game(int w = 20, int h = 10)
    {
        numberOfLanes = h;
        width = w;
        for (int i = 0; i < numberOfLanes; i++)
        {
            map.push_back(new Lane(width));
        }
        player = new Player(width);
        srand(time(NULL));
        InitGraphics();
    }
    ~Game()
    {
        delete player;
        for (Lane *laneptr : map)
            delete laneptr;
        TearDownGraphics();
    }
    void Run()
    {
        while (!quit)
        {
            Input();
            Draw();
            Logic();
        }
        ShowGameOver();
    }
};

int main()
{
    int width = 100;  // size of lanes
    int height = 30; // number of lanes
    Game game(width, height);
    game.Run();
}