// To delay method to slow down: delay(5000);
// Fill the led panel with color: matrix.fillScreen(BLACK.to_333());
// Print on the screen: Serial.println();
#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK  = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// global constant for the number of Invaders in the game
const int NUM_ENEMIES = 16;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// the following functions are for printing messages
void print_level(int level);
void print_lives(int lives);
void clear_screen();
void game_over();

class Color {
  public:
    int red;
    int green;
    int blue;
    Color() {
      red = 0;
      green = 0;
      blue = 0;
    }
    Color(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    uint16_t to_333() const {
      return matrix.Color333(red, green, blue);
    }
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);
const Color colors[] = {BLACK, RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, WHITE, LIME,AQUA};

class Invader {
  public:
    // Constructors
    Invader() {
      x = 0;
      y = 0;
      strength = 0;
    }
    // sets values for private date members x and y
    Invader(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    // sets values for private data members
    Invader(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    // sets values for private data members
    void initialize(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_strength() const {
      return strength;
    }

    // setters
    void set_x(int x_val) {
      x = x_val;
    }

    // Moves the Invader down the screen by one row
    // Modifies: y
    void move() {
      erase();
      y += 1;
      draw();
    }
    
    void random_move() {
      erase();
      y += 1;
      x = random(2, matrix.width() - 2);
      draw();
    }

    // draws the Invader if its strength is greater than 0
    // calls: draw_with_rgb
    void draw() {
      if (strength > 0) {
        draw_with_rgb(colors[strength], BLUE);
      }
    }
    
    // draws black where the Invader used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK, BLACK);
    }    
    
    // Invader is hit by a Cannonball.
    // Modifies: strength
    // calls: draw, erase
    void hit(int ball_strength) {
      if (strength == 0) {
        initialize(0, 0, 0);
      }
      erase();
      strength -= ball_strength;
      draw();
    }

    // checks if invader hits the end
    bool hit_wall(){
      if (y + 1 == matrix.height() - 1) {
        return true;
      }
      return false;
    }

    // checks if invader is dead
    bool is_dead() {
      if (strength <= 0) {
        return true;
      }
      return false;
    }

  private:
    int x;
    int y;
    int strength;
    
    // draws the Invader
    void draw_with_rgb(Color body_color, Color eye_color) {
      matrix.drawPixel(x, y - 2, body_color.to_333());
      matrix.drawPixel(x + 1, y - 2, body_color.to_333());

      matrix.drawPixel(x - 1, y - 1, body_color.to_333());
      matrix.drawPixel(x, y - 1, eye_color.to_333());
      matrix.drawPixel(x + 1, y - 1, eye_color.to_333());
      matrix.drawPixel(x + 2, y - 1, body_color.to_333());

      matrix.drawPixel(x - 1, y, body_color.to_333());
      matrix.drawPixel(x, y, body_color.to_333());
      matrix.drawPixel(x + 1, y, body_color.to_333());
      matrix.drawPixel(x + 2, y, body_color.to_333());

      matrix.drawPixel(x - 1, y + 1, body_color.to_333());
      matrix.drawPixel(x + 2, y + 1, body_color.to_333());
    }
};

class Cannonball {
  public:
    Cannonball() {
      x = 0;
      y = 0;
      strength = 1;
      fired = false;
    }
    
    // resets private data members to initial values
    void reset() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_strength() {
      return strength;
    }
    bool has_been_fired() const {
      return fired;
    }
    
    // sets private data members
    void set_strength(int strength_val) {
      strength = strength_val;
    }
    void set_x(int x_val) {
      x = x_val;
    }

    void fire(int x_arg, int y_arg) {
        x = x_arg;
        y = y_arg;
        fired = true;
    }
    
    // moves the Cannonball and detects if it goes off the screen
    // Modifies: y, fired
    void move(bool is_player) {
      if (y < 0 || y >= matrix.height()) {
        reset();
      }
      else {
        erase();
        if (is_player){
          y -= 1;
        }
        else {
          y += 1;
        }
        draw();  
      }  
    }
    
    // resets private data members to initial values
    void hit() {
      reset();
    }
    
    // draws the Cannonball, if it is fired
    void draw() {
      matrix.drawPixel(x,  y, colors[strength].to_333());
    }
    
    // draws black where the Cannonball used to be
    void erase() {
       matrix.drawPixel(x, y, BLACK.to_333());
    }

  private:
    int x;
    int y;
    int strength;
    bool fired;
};

class Player {
  public:
    Player() {
      initialize(1,14);
      lives = 3;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_lives() const {
      return lives;
    }
    
    // setter
    void set_x(int x_arg) {
      x = x_arg;
    }
    void set_y(int y_arg) {
      y = y_arg;
    }
    void set_lives(int lives_arg) {
      lives = lives_arg;
    }
    
    // Modifies: lives
    void die() {
      initialize(1, 14);
      lives -= 1;
    }
    
    // draws the Player
    // calls: draw_with_rgb
    void draw() {
      draw_with_rgb(colors[lives]);
    }
    
    // draws black where the Player used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK);
    }

    void move(int potentiometer_value) {
      int dotX = map(potentiometer_value, 0, 1023, 1, matrix.width() - 2);
      erase();
      set_x(dotX);
      draw();
    }

    bool is_defeated(Invader &enemy) {
      // when player hit by invader
      if (y == enemy.get_y() + 1) {
          if (x == enemy.get_x() - 1 || x == enemy.get_x() + 2) {
            die();
            return true;
          }
      }
      // when invader reaches the end
      else if (enemy.hit_wall()) {
        die();
        return true;
      }

      return false;
    }

    bool is_dead() {
      if (lives == 0) {
        return true;
      }
      return false;
    }

  private:
    int x;
    int y;
    int lives;

    // sets private data members x and y to initial values
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    // draws the player
    void draw_with_rgb(Color color) {
      matrix.drawPixel(x, y, color.to_333()); 
      matrix.drawPixel(x, y + 1, color.to_333());
      matrix.drawPixel(x - 1, y + 1,color.to_333()); 
      matrix.drawPixel(x + 1, y + 1,color.to_333()); 
    }
};

class Reward {
  public:
    // default constructor
    Reward() {
      x = 0;
      y = 1;
      // type 1 is mine(dangerous), 2 updates cannon ball strength,  3 updates cannon ball speed.
      type = 1;
      activated = false;
    }
    // getters
    int get_x() {
      return x;
    }
    int get_y() {
      return y;
    }
    int get_type() {
      return type;
    }
    int get_activated() {
      return activated;
    }

    //setters
    void set_x(int x_val) {
      x = x_val;
    }
    void set_y(int y_val) {
      y = y_val;
    }
    void set_type(int type_val) {
      type = type_val;
    }
    void set_ativated(bool act) {
      activated = act;
    }
    void reset() {
      x = 0;
      y = 1;
      activated = false;
    }

    // generate random number for x coordiate and type and activate moving reward by setting activated set to true
    void activate() {
      activated = true;
      //generates 1, 2, or 3 randomly
      type = random(1, 4);
      x = random(0, matrix.width() - 2);
    }
    // move reward
    void move() {
      if (y >= matrix.width()) {
        reset();
      }
      else {
        erase();
        y += 1;
        draw();
      }
    }
    // draw reward 
    void draw() {
      matrix.drawPixel(x, y, colors[type].to_333());
      matrix.drawPixel(x + 1, y, colors[type].to_333());

      matrix.drawPixel(x, y - 1, colors[type].to_333());
      matrix.drawPixel(x + 1, y - 1, colors[type].to_333());
    }
    // erase reward object
    void erase() {
      matrix.drawPixel(x, y, BLACK.to_333());
      matrix.drawPixel(x + 1, y, BLACK.to_333());

      matrix.drawPixel(x, y - 1, BLACK.to_333());
      matrix.drawPixel(x + 1, y - 1, BLACK.to_333());
    }

    // checks if reward get shot by cannon ball
    bool hit_ball(Cannonball &ball) {
      if (y == ball.get_y()) { 
        if (x == ball.get_x() || x + 1 == ball.get_x()) {
          erase();
          reset();
          ball.erase();
          return true;
        }
      }
      return false;
    }

  private:
    int x;
    int y;
    int type;
    bool activated;
};

class Game {
  public:
    Game() {
      level = -1;
      time_invader_move = millis();
      time_boss_move = millis();
      time_draw_invaders = millis();
      time_ball_player = millis();
      time_ball_invader = millis();
      time_player_move = millis();
      time_reward_appear = millis();
      time_reward_move = millis();

      time_period_invader = 0;
      time_period_ball_player = 20;
      time_period_reward_appear = 0;
      duration_power_up = 0;
      duration_speed_up = 0;

      boss_index = 3;
      intensity = 0;
    }

    // setters
    void set_intensity(int intensity_val) {
      intensity = intensity_val;
    }
    void set_level(int level_val) {
      level = level_val;
    }

    // getters
    int get_intensity() {
      return intensity;
    }
    int get_level() {
      return level;
    }
    
    // sets up a new game of Space Invaders
    // Modifies: global variable matrix
    void setupGame() {
      int y = 2;
      int x = 1;
      int strength = 1;

      if (level == -1) {
        for(int i = 0; i < NUM_ENEMIES; ++i){
            enemies[i]= Invader(0, 0, 0);
        }
        enemies[0] = Invader(2, 9, 1);
        enemies[1] = Invader(13, 9, 2);
        enemies[2] = Invader(25, 9, 3);
      }

      else if (level == 1) {
        for(int i = 0; i < NUM_ENEMIES / 2; ++i){
            enemies[i]= Invader(x, y, intensity);
            x += 4;
        }
        for(int i = NUM_ENEMIES / 2; i < NUM_ENEMIES; ++i){
            enemies[i]= Invader(0, 0, 0);
            x += 4;
        }
      }

      else if (level == 2) {
        for (int i = 0; i < NUM_ENEMIES; i++) {
          if ((strength > 2)) {
              strength -= 2;
          }
          if (i == 8) {
              strength++;
              x = 1;
              y += 4;
          }
          enemies[i] = Invader(x, y, strength + intensity - 1);
          x += 4;
          strength ++;
        }
      }

      else if (level == 3) {
        for (int i = 0 ; i < NUM_ENEMIES; i++) {
          strength = (i % 5) + 1;
          enemies[i] = Invader(x, y, strength + intensity - 1);
          x += 4;
          if (i == 7) {
            x = 1;
            y += 4;
          }
        }

      }

      else if (level == 4) {
        for (int i = 0; i < NUM_ENEMIES; i++) {

          if (i == NUM_ENEMIES / 2) {
              x = 1;
              y += 4;
            }

          if (i < NUM_ENEMIES / 2) {
            if (i % 2 == 0) {
              enemies[i] = Invader(x, y, 2 + intensity - 1); 
            }
            else {
              enemies[i] = Invader(x, y, 3 + intensity - 1);
            }
          }
          else {
            if (i % 2 == 0) {
              enemies[i] = Invader(x, y, 5 + intensity - 1); 
            }
            else {
              enemies[i] = Invader(x, y, 4 + intensity - 1);
            }
          }
          x += 4;
        }
      }
      else if (level != 0 && level % 5 == 0) {
        for (int i = 0; i < NUM_ENEMIES; i++) {
          if (i < NUM_ENEMIES / 2) {
            if (i == boss_index) {
              enemies[i] = Invader(x, y, 9);
            }
            else {
              enemies[i] = Invader(x, y, 0);
            }   
          }
          else {
            if (i == NUM_ENEMIES / 2) {
              x = 1;
              y += 4;
            }
            enemies[i] = Invader(x, y, random(intensity, 6 + intensity - 1));
          }
          x += 4;
        }
      }
      else {
        for (int i = 0 ; i < NUM_ENEMIES; i++) {
          enemies[i] = Invader(x, y, random(intensity, 6 + intensity - 1));
          x += 4;
          if (i == 7) {
            x = 1;
            y += 4;
          }
        }
      }

      // prints menu of new game
      if (level == -1) {
        print_menu();
      }
      else {
        // print level 
        clear_screen();
        print_level(level);
        // print lives of player
        clear_screen();
        print_lives(player.get_lives());
        clear_screen();
      }
    }
    
    // advances the game simulation one step and renders the graphics
    // see spec for details of game
    // Modifies: global variable matrix
    void update(int potentiometer_value, bool button_pressed) {
      // moves player
      move_player(potentiometer_value);
      // draw invaders
      draw_invaders();
      // move reward
      if (level != -1) {
        move_reward();
      }
      // player shoots cannon ball
      player_fire_cannon(button_pressed);

      // hits enemies
      hit_enemies();

      // invader shoots cannon ball every 5 rounds
      if (level % 5 == 0) {
        invader_fire_cannon();
      }

      // move invaders when level is not -1. 
      // when level is -1, prints out menu.
      if (level != -1) {
        if (left_invaders(NUM_ENEMIES / 2, NUM_ENEMIES - 1) > 0) {
          // moving first row
          move_invaders(NUM_ENEMIES / 2, NUM_ENEMIES - 1);
        }
        else {
          // moving second row
          if (level % 5 == 0) {
            move_invader_boss();
          }
          else {
            move_invaders(0, NUM_ENEMIES / 2 - 1);
          }     
        }
      }

      // reacts to reward
      reward_reaction();

      // checks if invaders defeat player
      for (int i = 0; i < NUM_ENEMIES; i++) {
        if (player.is_defeated(enemies[i])) {
          // when player is dead
          if (player.is_dead()) {
            // clear screen and print "Game over"
            clear_screen();
            game_over();
            // start a new game
            level = -1;
            player.set_lives(3);
          }
          ball_player.set_strength(1);
          set_speed_intensity();
          set_speed_level();
          reset_level();
        }
      }

      // checks if player get shot
      if (player_get_shot()) {
        player.die();
        // when player is dead
        if (player.is_dead()) {
          // clear screen and print "Game over"
          clear_screen();
          game_over();
          // start a new game
          level = -1;
          player.set_lives(3);
        }
        ball_player.set_strength(1);
        set_speed_intensity();
        set_speed_level();
        reset_level();
      }

      // checks if level cleared
      if (level_cleared()) {
        // when defeated boss, increase lives by 1
        if (level % 5 == 0) {
          if(player.get_lives() < 9) {
             player.set_lives(player.get_lives() + 1);
          }
        }
        // move to the next level
        level += 1;
        set_speed_level();
        reset_level();
      }
    }    

////////////////////////////////////////////////////////////////////////////////////////////////////////////// Methods for player. /////////////////////////////
////////////////////////////////////////////////////////////////////////////////

    // move player with potentiometer
    void move_player(int potentiometer_value) {
      if (millis() - time_player_move > 5) {
        player.move(potentiometer_value);
        time_player_move = millis();
      }
    }

    // player fire cannon ball with button
    void player_fire_cannon(bool button_pressed) {
      // fire cannon ball
      if (button_pressed && !ball_player.has_been_fired()) {
        ball_player.fire(player.get_x(), player.get_y());
      }
      // move cannon ball
      if (millis() - time_ball_player > time_period_ball_player) {
        if (ball_player.has_been_fired()) {
          ball_player.move(true);
        }
        time_ball_player = millis();
      }
    }

    // checks if player gets shot
    bool player_get_shot() {
      if (ball_invader.get_x() == player.get_x() && ball_invader.get_y() == player.get_y()) {
        ball_invader.reset();
        return true;
      }
      else if(ball_invader.get_x() == player.get_x() - 1 && ball_invader.get_y() == player.get_y() + 1) {
        ball_invader.reset();
        return true;
      }
      else if(ball_invader.get_x() == player.get_x() + 1 && ball_invader.get_y() == player.get_y() + 1) {
        ball_invader.reset();
        return true;
      }
      return false;
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////// Methods for invaders. /////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    // move invaders
    void move_invaders(int start, int end) {
      // move invaders
      if (millis() - time_invader_move > time_period_invader) {
        for (int i = start; i <= end; i++) {
          enemies[i].move();
        }
        time_invader_move = millis();
      }
    }

    // move invader boss
    void move_invader_boss() {
      // move boss randomly
      if(millis() - time_boss_move > time_period_invader) {
        enemies[boss_index].random_move();
        time_boss_move = millis();
      }
    }

    // returns number of left enemies in the range of start and end of index
    int left_invaders(int start, int end) {
      int counter = 0;
      for (int i = start; i <= end; i++) {
          if (!(enemies[i].is_dead())) {
            counter += 1;
          }
      }
      return counter;
    }

  // every five rounds, boss of invader shoots at player
    void invader_fire_cannon() {
      if (!ball_invader.has_been_fired()) {
        ball_invader.fire(enemies[boss_index].get_x(), enemies[boss_index].get_y());
      }

      int player_x = player.get_x();
      // move cannon ball
      if (millis() - time_ball_invader > 200) {
        if (ball_invader.has_been_fired()) {
          ball_invader.move(false);
        }
        time_ball_invader = millis();
      }
    }

    // draws invaders
    void draw_invaders() {
      if(millis() - time_draw_invaders > 200) {
        for (int i = 0; i < NUM_ENEMIES; i++) {
          enemies[i].draw();
        } 
        time_draw_invaders = millis();
      }
    }

    // checks if the enemy get shot
    void hit_enemies() {
      for (int i = 0; i < NUM_ENEMIES; i++) {
        if (ball_player.get_y() == enemies[i].get_y() && enemies[i].get_strength() > 0) {
          if (ball_player.get_x() == enemies[i].get_x() || ball_player.get_x() == enemies[i].get_x() + 1) {
            //erase cannon ball when it hit enemy.
            ball_player.erase();
            ball_player.reset();
            //prints menu and set intensity of game at initial moment of game 
            if (level == -1) {
              intensity = i + 1;
              level = 1;
              set_speed_intensity();
              setupGame();
            }
            else {
              enemies[i].hit(ball_player.get_strength());
            }
          }
        }
        else if (ball_player.get_y() == enemies[i].get_y() + 1 && enemies[i].get_strength() > 0) {
            if (ball_player.get_x() == enemies[i].get_x() - 1 || ball_player.get_x() == enemies[i].get_x() + 2) {
              //erase cannon ball when it hit enemy.
              ball_player.erase();
              ball_player.reset();
              //prints menu and set intensity of game at initial moment of game 
              if (level == -1) {
                intensity = i + 1;
                level = 1;
                set_speed_intensity();
                setupGame();
              }
              else {
                enemies[i].hit(ball_player.get_strength());
              }
              
            }
        }
      }
    }

    // sets speed of objects with respect to intensity
    void set_speed_intensity() {
      if (intensity == 1) {
        time_period_invader = 1200;
        time_period_ball_player = 40;
        time_period_reward_appear = 200;
      }
      else if (intensity == 2) {
        time_period_invader = 1100;
        time_period_ball_player = 35;
        time_period_reward_appear = 100;
      }
      else if (intensity == 3) {
        time_period_invader = 1000;
        time_period_ball_player = 25;
        time_period_reward_appear = 70;
      }
    }

    // set speed by level
    void set_speed_level(){
     // as level goes up, the speed of enemies increase
      if (time_period_invader - 10 * (level - 1) > 500) {
        time_period_invader -= 10 * (level - 1);
      }

      // as level goes up, the speed of cannon ball increase
      if (time_period_ball_player - (level / 2) > 20) {
        time_period_ball_player - (level / 2);
      }  

      // as level goes up, the speed of reward's appearing increase
      if (time_period_reward_appear - 5 * (level - 1) > 50) {
        time_period_reward_appear -= (level - 1);
      }  
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////// Methods for rewards. //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    // move reward
    void move_reward() {
      // activates a new movement
      if (millis() - time_reward_appear > time_period_reward_appear) {
        if (!reward.get_activated()) {
          reward.activate();
        }
        time_reward_appear = millis();
      }
      // move reward
      if (millis() - time_reward_move > 200) {
        if (reward.get_activated()) {
          reward.move();
        }
        time_reward_move = millis();
      }
    }

    // updates cannon ball strength or speed with respect to reward type when cannon ball it the reward
    void reward_reaction() {
      // checks if cannon ball hits reward
      if (reward.hit_ball(ball_player)){
        // make invaders move faster
        if (reward.get_type() == 1) {
          // speed up invaders
          time_period_invader = 500;
        }
        // strength update
        else if (reward.get_type() == 2) {
          // increase ball power from 1 to 2
          ball_player.set_strength(2);
          // when the first gain of reward, set clock
          if(duration_power_up == 0) {
            time_power_up_begin = millis();
          }
          // increases the power-up effect duration by 5 seconds
          if (duration_power_up < 40000) {
            duration_power_up += 8000;
          }
        }
        // speed update
        else if (reward.get_type() == 3) {
          time_period_ball_player = 12;
          // when the first gain of reward, set clock
          if(duration_speed_up == 0) {
            time_speed_up_begin = millis();
          }
          // increases the speed-up effect duration by 5 seconds
          if (duration_speed_up < 40000) {
            duration_speed_up += 8000;
          }  
        }
      }

      // invalidates power-up reward after the duration
      if (millis() - time_power_up_begin > duration_power_up) {
        ball_player.set_strength(1);
        duration_power_up = 0;
      }
      // invalidates speed-up reward after the duration
      if (millis() - time_speed_up_begin > duration_speed_up) {
        set_speed_intensity();
        duration_speed_up = 0;
      }
    }
  
//////////////////////////////////////////////////////////////////////////////////////////////////////////// Methods for menu. /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    void print_menu() {
      clear_screen();
      matrix.setCursor(0, 0);
      matrix.print("New  Game");
      delay(2000);
      clear_screen();
      matrix.setCursor(0, 0);
      matrix.print("Pick Level");
      delay(2000);
      clear_screen();
      matrix.setCursor(0, 0);
      matrix.print("1 2 3");
    }

  private:
    int level;
    // stores time every move of reward begins
    unsigned long time_reward_move;
    // stores time when reward appears
    unsigned long time_reward_appear;
    // defines time space between reward appears
    unsigned long time_period_reward_appear;

    // stores time when power-up reward awarded
    unsigned long time_power_up_begin;
    // stores duration of power-up reward's lasting
    unsigned long duration_power_up;
    // stores time when speed-up reward awarded
    unsigned long time_speed_up_begin;
    // stores duration of speed-up rewar's lasting
    unsigned long duration_speed_up;

    // stores time every move of invader begins
    unsigned long time_invader_move;
    // time period invader moves
    unsigned long time_period_invader;

    // stores time when every move of player's cannon ball begins
    unsigned long time_ball_player;
    // time period ball of player moves
    unsigned long time_period_ball_player;
    // stores time when every move of invader's cannon ball begins
    unsigned long time_ball_invader;

    // time when drawing invaders
    unsigned long time_draw_invaders;

    // stores time every move of player begins
    unsigned long time_player_move;

    // stores time every move of boss begins
    unsigned long time_boss_move;

    // index of boss in enemies array
    int boss_index;

    // determine intensity of game
    int intensity;

    Player player;
    Cannonball ball_player;
    Cannonball ball_invader;
    Reward reward;
    Invader enemies[NUM_ENEMIES];

    // check if Player defeated all Invaders in current level
    bool level_cleared() {
      if (left_invaders(0, NUM_ENEMIES - 1) == 0) {
        return true;
      }
      return false;
    }

    // set up a level
    void reset_level() {
      time_period_invader = 2000;
      reward.reset();
      ball_player.reset();
      ball_invader.reset();
      setupGame();
      draw_invaders();
    }
};

// a global variable that represents the game Space Invaders
Game game;
// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  Serial.begin(9600);
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
  game.setupGame();
  game.draw_invaders();
}


// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH); 
  game.update(potentiometer_value, button_pressed);
}

// displays Level
void print_level(int level) {
  matrix.setCursor(0, 0);
  matrix.setTextSize(1);
  matrix.print("Round: " + String(level));
  delay(2000);
}

// displays number of lives
void print_lives(int lives) {
  matrix.setCursor(0, 0);
  matrix.setTextSize(1);
  matrix.print("Lives: " + String(lives));
  delay(2000);
}

// displays "game over"
void game_over() {
  matrix.setCursor(0, 0);
  matrix.setTextSize(1);
  matrix.print("Game over");
  delay(2000);
}

// clear screen
void clear_screen() {
  for (int row = 0;  row < matrix.width(); row++) {
    for (int col = 0; col < matrix.height(); col++) {
      matrix.drawPixel(row, col, BLACK.to_333());
    }
  }
}