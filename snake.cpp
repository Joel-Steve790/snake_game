#include<iostream>
#include<raylib.h>
#include<raymath.h>
#include<deque>

using namespace std;

//windows grid values
int cellSize = 20;
int cellCount = 40;
Color darkGreen = {0, 117, 44, 255};
int offset = 75;

// A stopwatch tracking the last time the snake moved.
double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> &deque)
{
    for(unsigned int i=0; i< deque.size(); i++)
    {
      if(Vector2Equals(element, deque[i])){
          return true;
      } 
    }
    return false;
}

bool eventTriggered(double interval){

  double currentTime = GetTime();  // Check the exact time right now.

  // If the time passed since the last move is greater than or equal to our interval (0.2s)
  if (currentTime - lastUpdateTime >= interval)  //interval = 0.2
  {
    lastUpdateTime = currentTime; //like resetting the stopwatch - set the last update time to current time
    return true; //tell the game: "Yes, it's time to move!"
  }
  return false;  // Tell the game: "Not yet, keep waiting."
}

class Snake {
public:
    //snake body initial points
    deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
    Vector2 direction = {};
    bool addSegment = false;  //become true if the snake head point covers the food position

    //snake body
    void Draw(){
        for (unsigned int i =0; i < body.size(); i++){
            float x = body[i].x;
            float y = body[i].y;
            Rectangle rec = {offset + x*cellSize, offset + y*cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(rec, 0.5, 6, BLACK );
        }
    }
    
    //moving the snake
    void Update(){

      if(addSegment == true)
      {
        body.push_front(Vector2Add(body[0], direction));
        addSegment = false;
      }
      else
      {
      body.pop_back();
      body.push_front(Vector2Add(body[0], direction));
      }
    }

    void Reset()
    {
      body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9} };
      direction = {1,0};
    }
};

class Food {
public:
  Vector2 food_pos = {};
  Texture2D texture;
  
  //constructor - for loading image texture
  Food(deque<Vector2> snakeBody) 
  {
    Image image = LoadImage("Graphics/apple1.png");
    ImageResize(&image, cellSize, cellSize);
    texture = LoadTextureFromImage(image);
    UnloadImage(image);
    food_pos = GenerateRandomPos(snakeBody);
  }

  //destructor - after the texture load
  ~Food()
  {
    UnloadTexture(texture);
  }

  void Draw(){
    //DrawRectangle(food_pos.x * cellSize, food_pos.y * cellSize, cellSize, cellSize, WHITE);
    DrawTexture(texture, offset + food_pos.x * cellSize, offset + food_pos.y * cellSize, WHITE);
  }
  
  Vector2 GenerateRandomCell()
  {
    //use GetRandomValue() method to get random x and y values
    float x = GetRandomValue(0, cellCount - 1);
    float y = GetRandomValue(0, cellCount - 1);
    return Vector2{x, y};
  }

  Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
  {
    Vector2 food_pos = GenerateRandomCell();
    //if the position of snake and food are same - then generate a position again
    while (ElementInDeque(food_pos, snakeBody))
    {
      food_pos = GenerateRandomCell();
    }
    return food_pos;
    
  }
};

class Game {
public:
    int score;
    Snake snake{};
    Food food{snake.body};

    bool running = true;

    void Draw()
    {
      snake.Draw();
      food.Draw();
    }

    void Update()
    {
      if(running)
      {
          snake.Update();
          CheckFoodCollision();
          CheckEdgeCollision();
          CheckSnakeCollision();

      }
    }

    void CheckFoodCollision(){
      if(Vector2Equals(snake.body[0], food.food_pos))
      {
        food.food_pos = food.GenerateRandomPos(snake.body);
        Score();
        snake.addSegment = true;
      }

    }

    void CheckEdgeCollision()
    {
      if(snake.body[0].x == cellCount || snake.body[0].x ==-1 )
      {
        GameOver();
      }
      if(snake.body[0].y == cellCount || snake.body[0].y ==-1 )
      {
        GameOver();
      }
    }

    void CheckSnakeCollision()
    {
      deque<Vector2> headlessBody = snake.body;
      headlessBody.pop_front();
      if(ElementInDeque(snake.body[0], headlessBody))
      {
        GameOver();
      }
    }

    void GameOver()
    {
      snake.Reset();
      food.food_pos = food.GenerateRandomPos(snake.body);
      running = false;
      score = 0;
    }

    void Score()
    {
        score +=8;
        cout << score << endl;
      }
    
};

int main() {

    InitWindow(2*offset + cellSize*cellCount, 2*offset + cellSize*cellCount, "My Snake Game");
    SetTargetFPS(60);
    
    Game game{};
    //code loop - 
    while (WindowShouldClose() == false) {

        BeginDrawing();

        if(eventTriggered(0.1))
        {
          game.Update();
        }

        //snake movement control
        if(IsKeyPressed(KEY_W) && game.snake.direction.y != 1)
        {
          game.snake.direction = {0, -1};
          game.running = true;
  
        }
        if((IsKeyPressed(KEY_A) && game.snake.direction.x != 1) || (IsKeyPressed(KEY_W) && IsKeyPressed(KEY_D))) 
        {
          game.snake.direction = {-1, 0};
          game.running = true;
          
        }
        if(IsKeyPressed(KEY_S) && game.snake.direction.y != -1) 
        {
          game.snake.direction = {0, 1};
          game.running = true;
          
        }
        if((IsKeyPressed(KEY_D) && game.snake.direction.x != -1) || (IsKeyPressed(KEY_W) && IsKeyPressed(KEY_A))) 
        {
          game.snake.direction = {1, 0};
          game.running = true;
          
        }

        ClearBackground(darkGreen);\
        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize * cellCount+10, (float)cellSize * cellCount+10}, 5, BLACK);
        DrawText(TextFormat("%i", game.score), 395, 5, 80, BLACK);
        //function calls
        game.Draw();

        EndDrawing();
    } 

    CloseWindow();
    return 0;
}
