#include <HT1632.h>
#include <font_5x4.h>
#include <images.h>

#define WIDTH OUT_SIZE
#define HEIGHT COM_SIZE

int stage = 0;
int counter = 0;
int started = counter;

#define UNDEAD_HIST_LEN 8
#define MAX_UNCHANGED_CYCLES 30
int undead_hist[UNDEAD_HIST_LEN];
int undead_unchanged = 0;

void drawStart(int amount) {
  //HT1632.clear();
  HT1632.selectChannel(stage);
  
  /*T1632.drawImage(IMG_SAILBOAT, IMG_SAILBOAT_WIDTH,  IMG_SAILBOAT_HEIGHT, 16, 4);
  HT1632.drawImage(IMG_MAIL, IMG_MAIL_WIDTH, IMG_MAIL_HEIGHT, 2, 6);*/
  //HT1632.drawImage(IMG_TWITTER, IMG_TWITTER_WIDTH, IMG_TWITTER_HEIGHT, 10, 5);
  
  Serial.print("seed: ");
  Serial.println(counter);
  
  started = counter;
  randomSeed(counter);  
  Serial.print("new pixel: ");
  Serial.print(amount);
  while( amount <= 0 )
    amount = ((WIDTH*HEIGHT) * 50)/random(55, 120) - amount;
  
  Serial.print(" => ");
  Serial.println(amount);
  
  uint8_t x, y;
  for( int r = 0; r < amount; ++r ) {
    //Serial.print("r: ");
    //Serial.print(r);
    
    // every 5 pixel a new position
    if( r % 5 == 0 ) {
      x = random(0, WIDTH);
      y = random(0, HEIGHT);
      //Serial.print(" x: "); Serial.print(x);
      //Serial.print(" y: "); Serial.print(y);
    }
    
    HT1632.setPixel(
      x + random(-((r % 4)+1), ((r % 4)+1)), 
      y + random(-((r % 4)+1), ((r % 4)+1)) );
    
    //Serial.println();
  }
    
}

int evolve() {
  
  uint8_t x, y, n, x1, y1;
  int undead = 0;
  uint8_t nstage = (stage+1) % 2;
  
  for( x = 0; x < WIDTH; ++x )
    for( y = 0; y < HEIGHT; ++y) {
      n = 0;
      
      for( y1 = y - 1; y1 <= y + 1; y1++ )
        for( x1 = x - 1; x1 <= x + 1; x1++ )
          if( HT1632.getPixel( (x1 + WIDTH) % WIDTH, (y1 + HEIGHT) % HEIGHT, stage ) )
            n++;
 
      if( HT1632.getPixel(x, y, stage) )
        n--;
      if( n == 3 || ( n == 2 && HT1632.getPixel(x, y) ) ) {
        HT1632.setPixel(x, y, nstage);
        undead++;
      }
      else
        HT1632.clearPixel(x, y, nstage);
      //new[y][x] = (n == 3 || (n == 2 && univ[y][x]));
      
    }
  
  stage = nstage;
  return undead;
}

void setup () {
  Serial.begin(9600);
  Serial.println("Sartup");
  
  counter = 42;//800 + analogRead(0);
  
  //HT1632.setCLK(13);
  //HT1632.begin(12, 10, 9);
  //HT1632.beginDaisyChain(2, 12, 13, 10, 9);
  
  HT1632.setCLK(5);
  HT1632.begin(4, 6, 7);
  
  HT1632.setBrightness(16);
  HT1632.fillAll();
  HT1632.render();
  
  delay(200);
  
  HT1632.clear();
  drawStart(0);
  HT1632.render();
  delay(2000);
}

void loop () {
  
  //Serial.print("counter: ");
  //Serial.println(counter);
  counter++;
  uint8_t undead = evolve();
  /*for( uint8_t r = 0; r < random(0, 10); ++r )
    HT1632.setPixel(random(0, WIDTH), random(0, HEIGHT), stage);*/
  
  HT1632.renderTarget(0);
  HT1632.render();
  //HT1632.renderTarget(1);
  //HT1632.render();
  //delay(10);
  
  if( 1==2 && undead < 15 && counter - started >= 10 ) {
    Serial.print("cycles: ");
    Serial.println(counter - started);
    
    drawStart((15 - undead) * 4);
    HT1632.render();
    
    if( undead == 0 )
      delay(200);
  }
  
  uint8_t i = 0;
  for(i = 0; i <= UNDEAD_HIST_LEN; i++ ) 
    if( i < UNDEAD_HIST_LEN && undead == undead_hist[i] ) {
      undead_unchanged++;
      break;
    }
    else if( i == UNDEAD_HIST_LEN )
      undead_unchanged = 0;
      
  undead_hist[UNDEAD_HIST_LEN-1] = undead;
  for(i = 1; i < UNDEAD_HIST_LEN; i++ ) 
    undead_hist[i-1] = undead_hist[i];
    
  //Serial.print("undead_unchanged: ");
  //Serial.println(undead_unchanged);
  
  if( undead_unchanged >= MAX_UNCHANGED_CYCLES && counter - started >= 20 ) {
    Serial.println("undead unchanged!");
    Serial.print("cycles: ");
    Serial.print(counter - started);
    Serial.print("  w/o unchanged: ");
    Serial.println(counter - started - MAX_UNCHANGED_CYCLES);
    
    drawStart( -undead );
    HT1632.render();
    delay(200);
  }
  
}
