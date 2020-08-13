#include <iostream>
#include <fstream>
#include <random>
#include <time.h>
#include <vector>
#include <windows.h>

#define forI(numLoops) for (int i = 0; i < numLoops; i++)

///Grid info
const char adultMale = 'M', juvenileMale = 'm', adultFemale = 'F', juvenileFemale = 'f', radioactiveBunny = 'X', emptySpace = 'O';

///Bunny info
const int BUNNY_ADULT_AGE = 2; //Age that bunnies become adults
const int BUNNY_COLOR_NUM = 4;
const int BUNNY_NAMES_NUM = 30;
const int GRID_WIDTH = 20, GRID_HEIGHT = 20;

const int timeBetweenAnnouncements = 1000;
const int timeBetweenTurns = 1000;
const int twoPercentChance = 500; //Used for calculating 2 percent chance of radioactive bunny
const bool male = 0, female = 1;
std::string bunny_colors[BUNNY_COLOR_NUM] = {"black", "white", "brown", "spotted"};

const int black = 0, white = 1, brown = 2, spotted = 3; //Position of color string in bunny_colors
std::string bunny_names[BUNNY_NAMES_NUM] = {"Carrot", "Lettuce", "Cabbage", "Yodler", "Fin", "Grouch", "Happy", "Angry",
"Sneezy", "Hungry", "The Beast", "Destroyer", "Friendly", "Bubkiss", "Drunk", "Satan", "Ecstatic", "See plus plus", "Ocean",
"Britches", "Mauser", "Nike", "Shopper", "Brilliant", "Cancer", "Applebee", "Pudding", "Pie", "Penny", "Prudence"};

struct bunny_node{ //For a singly linked list of Bunnies
    int widthPos, heightPos;
    bool sex = rand() % 2; //random gender
    std::string color = bunny_colors[rand() % BUNNY_COLOR_NUM]; //random color
    int age = 0;
    std::string name = bunny_names[rand() % BUNNY_NAMES_NUM];
    bool radioactiveVampireMutantNinjaBunny = (rand() % twoPercentChance) == 0; //2% chance of being radioactive

    bunny_node* nextBunny;
};

class BunnyLinkedList{
private:
    char grid[GRID_WIDTH][GRID_HEIGHT];
    const int maxRabbitPop = 100;
    std::fstream new_file;

    bunny_node *head, *tail;

    int radioactiveBunniesNum = 0;
    int population = 0; //Total bunny population

    ///Grid manipulation functions
    bool GridPosFree(int posX, int posY){ //Checks to see if a specific spot on the grid has no other rabbits occupying
                                          //returns true if spot free and does not exceed bounds of grid
        if (posX >= 0 && posX < GRID_WIDTH &&
        posY >= 0 && posY < GRID_HEIGHT){
            bool free = grid[posX][posY] == emptySpace ? true : false;
            return free;
        }
        return false;
    }

    char GridPosCharIs(int posX, int posY){ //Returns what is occupying given grid pos
        if (posX >= 0 && posX < GRID_WIDTH &&
            posY >= 0 && posY < GRID_HEIGHT){
            return grid[posX][posY];
        }else{
            return 'n'; //Returns char that means nothing
        }
    }

    void AssignNewBunnyPos(bunny_node* new_bunny){ //When a new bunny is created, this is called, and will loop until a free position is called
        int posX, posY;
        do{
            posX = rand() % GRID_WIDTH;
            posY = rand() %  GRID_HEIGHT;
        }
        while (!GridPosFree(posX, posY));
        new_bunny->widthPos = posX;
        new_bunny->heightPos = posY;
    }

    void MoveBunnyPos(bunny_node* bunny){ //Is called in NewTurn loop for each bunny. Each bunny will move in a random direction
        const int up = 0, right = 1, down = 2, left = 3; //Movement position is randomized out of 4 possiblities. These define which way it will go for ease of reading
        const int NUM_MOVE_DIRECS = 4;
        const int maxMoveAttempts = 3; //Prevents infinite loop from occuring if there are no positions the bunny can move too. Must be more eloquent way, needs a rethink.
        int attemptsMade = 1; //Number of movement attempts
        int moveDirec;

        bool posFree = false;
        while (!posFree && attemptsMade <= maxMoveAttempts){
            moveDirec = rand() % NUM_MOVE_DIRECS;

            switch(moveDirec){
            case up:
                if (GridPosFree(bunny->widthPos, bunny->heightPos - 1)){
                    posFree = true;
                    grid[bunny->widthPos][bunny->heightPos] = emptySpace;
                    bunny->heightPos--;
                }
                attemptsMade++;
                break;

            case right:
                if (GridPosFree(bunny->widthPos + 1, bunny->heightPos)){
                    posFree = true;
                    grid[bunny->widthPos][bunny->heightPos] = emptySpace;
                    bunny->widthPos++;
                }
                attemptsMade++;
                break;

            case down:
                if (GridPosFree(bunny->widthPos, bunny->heightPos + 1)){
                    posFree = true;
                    grid[bunny->widthPos][bunny->heightPos] = emptySpace;
                    bunny->heightPos++;
                }
                attemptsMade++;
                break;

            if (GridPosFree(bunny->widthPos - 1, bunny->heightPos)){
                    posFree = true;
                    grid[bunny->widthPos][bunny->heightPos] = emptySpace;
                    bunny->widthPos--;
                }
                attemptsMade++;
                break;
            }
        }
    }

    void WriteDeathMessage(bunny_node *dead_bunny){
        std::string oString;
        if (dead_bunny->radioactiveVampireMutantNinjaBunny){
            oString = "Radioactive vampire mutant ninja bunny " + dead_bunny->name + " died!" + "\n";
        }else{
            oString = "Bunny " + dead_bunny->name + " died!" + "\n";
        }
        Write(oString);
    }

    void DisplayGrid(){ //Outputs to file and to console
        Write("\n");
        for (int i = 0; i < GRID_WIDTH; i++){
            std::string rowInfo;
            for (int j = 0; j < GRID_HEIGHT; j++){
                rowInfo += std::string(1, grid[i][j]); //Creates each row /////////////////////////
            }
            Write(rowInfo); //Writes each row to file and console
        }
        Write("\n");
    }

    void UpdateGrid(){ //Places correct symbols on the grid
        bunny_node *tmp = head;

        while (tmp != NULL){
            char bunnyType;
            if (tmp->radioactiveVampireMutantNinjaBunny){
                bunnyType = radioactiveBunniesNum;
            }else if (tmp->sex == male){
                bunnyType = tmp->age <= BUNNY_ADULT_AGE ? juvenileMale : adultMale;
            }else{
                bunnyType = tmp->age <= BUNNY_ADULT_AGE ? juvenileFemale : adultFemale;
            }

            grid[tmp->widthPos][tmp->heightPos] = bunnyType;
            tmp = tmp->nextBunny;
        }
    }

    void CreateGrid(){ //Creates a grid for the bunnies. Called once
        for (int i = 0; i < GRID_WIDTH; i++){
            for (int j = 0; j < GRID_HEIGHT; j++){
                grid[i][j] = emptySpace;
            }
        }
    }


    //Vampire convertion
    void ConvertToRadioactive(int posX, int posY){ //Finds a bunny with given position on the grid
        bunny_node *bunny = head;

        while (bunny != NULL){
            if (bunny->widthPos == posX && bunny->heightPos == posY){
                bunny->radioactiveVampireMutantNinjaBunny = true;
                break;
            }
            bunny = bunny->nextBunny;
        }
    }

    bool CanConvertToRadioactive(char gridChar){ //Checks if char is something that can be converty to radioactive
        if (gridChar != radioactiveBunny && gridChar != emptySpace && gridChar != 'n'){
            return true;
        }else{
            return false;
        }
    }

    void ShouldConvertToRadioactive(bunny_node* bunny){ //Radioactive bunnies will only convert bunnies 1 space away, left/right/up/down.
                                                        //Finds if there are spaces around given bunny, and if so, calls ConvertToRadioactive
        int x = bunny->widthPos, y = bunny->heightPos;
        char gridChar = GridPosCharIs(x, y - 1);
        if (CanConvertToRadioactive(gridChar)){ //Up
            ConvertToRadioactive(x, y - 1);
        }

        gridChar = GridPosCharIs(x + 1, y);
        if (gridChar != radioactiveBunny && gridChar != emptySpace && gridChar != 'n'){//Right
            ConvertToRadioactive(x + 1, y);
        }

        gridChar = GridPosCharIs(x - 1, y);
        if (gridChar != radioactiveBunny && gridChar != emptySpace && gridChar != 'n'){ //Left
            ConvertToRadioactive(x - 1, y);
        }

        gridChar = GridPosCharIs(x, y + 1);
        if (CanConvertToRadioactive(gridChar)){ //Up
            ConvertToRadioactive(x, y + 1);
        }
    }

    //Bunny birth
    void CanBeBorn(bunny_node* mother){ //If there is no available spot around the mother bunny, the baby will not be born
        if (GridPosFree(mother->widthPos, mother->heightPos - 1)){ //Up
            AddBunny(mother->color, mother->widthPos, mother->heightPos - 1);
        }else if(GridPosFree(mother->widthPos, mother->heightPos + 1)){ //Down
            AddBunny(mother->color, mother->widthPos, mother->heightPos + 1);
        }else if(GridPosFree(mother->widthPos - 1, mother->heightPos)){ //Left
            AddBunny(mother->color, mother->widthPos - 1, mother->heightPos);
        }else if(GridPosFree(mother->widthPos + 1, mother->heightPos)){ //Right
            AddBunny(mother->color, mother->widthPos + 1, mother->heightPos);
        }
    }

    ///LinkedList manipulation functions
    void Display(bunny_node *bunny){ //Cout's a bunny's info to the console
        std::string outputString = bunny->name + ": " + "age is " + std::to_string(bunny->age) + ", color is " + bunny->color + " ";
        bunny->sex == male ? outputString += "Male " : outputString += "Female ";
        bunny->radioactiveVampireMutantNinjaBunny == true ? outputString += "Radioactive " : outputString += "Normal ";

        Write(outputString);
    }

    void Breed(bunny_node *bunny){ //If a male over age 2 is present, creates a new bunny for each female over age 2
        bunny_node *tmp = head;

        bool malePresent = false;
        while (tmp != NULL){ //Checks for male
            if (tmp->sex == male && tmp->age > 2 && tmp->radioactiveVampireMutantNinjaBunny == false){
                malePresent = true;
                break;
            }
            tmp = tmp->nextBunny;
        }

        tmp = head;

        if (malePresent){
            while (tmp != NULL){
                if (tmp->sex == female && tmp->age > 2 && tmp->radioactiveVampireMutantNinjaBunny == false){
                    CanBeBorn(tmp);
                }
                tmp = tmp->nextBunny;
            }
        }

        malePresent = false;
    }

    void AddBunny(std::string bunnyColor = "", int posX = NULL, int posY = NULL){
        bunny_node *tmp = new bunny_node;

        if (posX == NULL && posY == NULL){
            AssignNewBunnyPos(tmp);
        }else{
            tmp->widthPos = posX;
            tmp->heightPos = posY;
        }

        if (!bunnyColor.empty()){ //Sets a specific color
            tmp->color = bunnyColor;
        }

        tmp->nextBunny = NULL;

        if (head == NULL){ //If linked list doesn't exist
            head = tmp;
            tail = tmp;
        }else{
            tail->nextBunny = tmp;
            tail = tail->nextBunny;
        }

        std::string outputString;
        if (tmp->radioactiveVampireMutantNinjaBunny){
            outputString = "Radioactive vampire mutant ninja bunny " + tmp->name + " was born!" + "\n";
        }else{
            outputString = "Bunny " + tmp->name + " was born!" + "\n";;
        }
        Write(outputString);

        population++;
    }

    void KillBunny(bunny_node *bunnyToDie){
        bunny_node *tmp;
        tmp = head;

        if (bunnyToDie == head){
            head = head->nextBunny;
            grid[tmp->widthPos][tmp->heightPos] = emptySpace;
            WriteDeathMessage(tmp);
            delete tmp;
        }else{
            bunny_node *priorNode = head;

            /*if (bunnyToDie == tail){
                tmp = tail;
                tail = NULL;
                grid[tmp->widthPos][tmp->heightPos] = emptySpace;
                delete tmp;
            }*/

            while(tmp != NULL){
                if (bunnyToDie == tmp){
                    tmp = bunnyToDie;
                    priorNode->nextBunny = tmp->nextBunny;
                    grid[tmp->widthPos][tmp->heightPos] = emptySpace;
                    WriteDeathMessage(tmp);
                    delete tmp;
                    break;
                }else{
                    priorNode = tmp;
                    tmp = tmp->nextBunny;
                }
            }
        }

        population--;
    }

    void PopulationCull(bool shouldCull = false){ //Checks if there are too many bunnies, and if so, kills about half
        /*bool shouldCull is changed to true if a different function orders a culling*/
        if (population >= maxRabbitPop || shouldCull){
            bunny_node *tmp = head;

            while (tmp != NULL){
                bool shouldDie = rand() % 2;
                if (shouldDie){
                    KillBunny(tmp);
                }

                tmp = tmp->nextBunny;
            }
            std::string outputString = "BUNNY POPULATION CULL HAS OCCURED";
            Write(outputString);
        }
    }

    void Write(std::string oString){//Writes all information and events to console and to a file
        Sleep(timeBetweenAnnouncements); //1 second delay between announcements
        std::cout << oString << std::endl;

        if (!new_file){
            std::cout << "File creation failed." << std::endl;
        }else{
            new_file << oString << "\n";
        }
    }

    void CheckUserInput(){ //Checks if user input should init a culling
        std::string initCull;

        std::cout <<std::endl << "Would you like to initiate a bunny culling?" << std::endl;
        std::cin >> initCull;

        if (initCull == "Yes" || initCull == "y" || initCull == "yes" || initCull == "Y"){
            PopulationCull(true);
        }
    }

public:
    BunnyLinkedList(){
        head = NULL;
        tail = NULL;
        new_file.open("colony_info.txt", std::fstream::out);

        CreateGrid();

        const int BUNNY_INIT_NUM = 5; //Number of bunnies created at sim start
        forI(BUNNY_INIT_NUM){
            AddBunny();
        }
    }

    bool ShouldColonyDie(){ //Checks if linked list is empty ie. last bunny has died and returns true if so
        bool popDead;
        head == NULL ? popDead = true : popDead = false;
        if (popDead){
            Write("All bunnies are dead!");
        }
        return popDead;
    }

    void NewTurn(){ //Each iteration of main loop executes here.
        bunny_node *tmp;
        tmp = head;

        Sleep(timeBetweenTurns); //2 seconds between each loop iteration

        std::cout << std::endl <<std::endl;
        Write("\n");

        while(tmp != NULL){
            if (tmp->radioactiveVampireMutantNinjaBunny){
                radioactiveBunniesNum++;
            }

            Display(tmp);
            MoveBunnyPos(tmp);

            if (tmp->radioactiveVampireMutantNinjaBunny){ //Causes deaths based on age
                if (tmp->age >= 50){
                    KillBunny(tmp);
                }
            }else if (tmp->age >= 10){
                KillBunny(tmp);
            }

            tmp->age++;
            tmp = tmp->nextBunny;
        }

        UpdateGrid();
        DisplayGrid();

        Breed(tmp);
        PopulationCull(); //Checks if there are too many bunnies

        forI(radioactiveBunniesNum){ //Changes a normal bunny into a radioactive one for each radioactive bunny present
            tmp = head;
            while(tmp != NULL){
                if (!tmp->radioactiveVampireMutantNinjaBunny){
                    tmp->radioactiveVampireMutantNinjaBunny = true;
                    break;
                }
                tmp = tmp->nextBunny;
            }
        }
        radioactiveBunniesNum = 0;

        CheckUserInput();
    }

};

int main(){
    srand(time(NULL));
    BunnyLinkedList Colony;

    bool simRunning = true;
    while (simRunning){ //Simulation loop
        Colony.NewTurn();
        ///Should simulation stop?
        if(Colony.ShouldColonyDie()){
            simRunning = false;
        }
    }

    return 0;
}
