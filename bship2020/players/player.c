#include <stdio.h>
#include <stdlib.h>
#include <my-ipc.h>
#include <client-side.h>
#include <redundant.h>
#include <public.h>

//戦略の意図
//守り編：どの船を一番隠しやすいか考えると、明らかにsであるから、sをなるべくみつからないような配置にする,つまり、大きい船は橋に寄せて、真ん中の広いスペースにsを４つ置く。
//攻撃編：基本的な実装は普通にやる、問題は特に情報がないときにどっから攻撃するかだが、全体のマス目とおける船の制約から考えて、端に置かれやすいと判断、端から攻撃するプラグラムを作成した。forを０からじゃなくて９から回すことで０から回すと全滅が遅くなるような配置メタにした。

const char myName[] = "03200242";
const char deployment[] = "Bc0d0e0f0 Ci4i5i6 Cc8d8e8 Da2a3 Da5a6 Dh2i2 Sd5 Sf2 Sg4 Sg6 ";

int max(int a,int b){
  if(a>=b){
    return a;
  }
  else{
    return b;
  }
}
int min(int a,int b){
  if(a>=b){
    return b;
  }
  else{
    return a;
  }
}
int kyori(int a,int b){
  return max(a-4,max(4-a,max(b-4,4-b)));
}
 


enum ship {
  UNKNOWN,
  ROCK,
  NOSHIP,
  BSHIP,
  CSHIP,
  DSHIP,
  SSHIP,
  TEMP
};

int cur_x,cur_y;
enum ship enemy_board[BD_SIZE][BD_SIZE];    // BD_SIZE is 9 (defined in public.h)

void respond_with_name(void)
{
  char *str = (char *)malloc(sizeof(myName));
  strcpy(str, myName);
  send_to_ref(str);
  free(str);
}

void respond_with_deployment(void)
{
  char *str = (char *)malloc(sizeof(deployment));
  strcpy(str, deployment);
  send_to_ref(str);
  free(str);
}


void init_board(void)
{
  //======kokokara======
  int ix, iy;

  for(ix = 0; ix < (BD_SIZE); ix++)
  {
    for(iy = 0; iy < (BD_SIZE); iy++)
    {
      //======kokokara======
      enemy_board[ix][iy]=UNKNOWN;	
      //======kokomade======
    }
  }

  //rock is out of bound
  //======kokokara======
  enemy_board[0][1] = ROCK;
  enemy_board[1][0] = ROCK;
  enemy_board[0][0] = ROCK;
  enemy_board[0][7] = ROCK;
  enemy_board[0][8] = ROCK;
  enemy_board[1][8] = ROCK;
  enemy_board[7][0] = ROCK;
  enemy_board[8][0] = ROCK;
  enemy_board[8][1] = ROCK;
  enemy_board[7][8] = ROCK;
  enemy_board[8][7] = ROCK;
  enemy_board[8][8] = ROCK;

  //======kokomade======
}

int flag=0;

void respond_with_shot(void)
{
  char shot_string[MSG_LEN];
  int x, y;


  if(flag==1){
    x = rand() % BD_SIZE;
    y = rand() % BD_SIZE;
    //=====kokokara=====
    while (TRUE)
    {
    x = rand() % BD_SIZE;
    y = rand() % BD_SIZE;
    //=====kokokara=====
    if(enemy_board[x][y] == TEMP){
      break;
    }
    }
  }
  else{
    int i,j;
    int ma=0;
    for(i=0;i<9;i++){
      for(j=0;j<9;j++){
	if(enemy_board[i][j]==UNKNOWN){
	  ma=max(ma,kyori(i,j));
	}
      }
    }
    for(i=8;i>=0;i--){
      for(j=8;j>=0;j--){
	if(kyori(i,j)==ma&&enemy_board[i][j]==UNKNOWN){
	  x=i;
	  y=j;
	}
      }
    }
  }
    
    


    //=====kokomade=====
  printf("[%s] shooting at %d%d ... ", myName, x, y);
  sprintf(shot_string, "%d%d", x, y);
  send_to_ref(shot_string);
  cur_x = x;
  cur_y = y;
}

void set_noship(int x, int y)
{
   //=====kokokara=====
   if(0<=x&&x<=8&&0<=y&&y<=8){
     enemy_board[x][y] = NOSHIP;
   }
   //=====kokomade=====
}


void set_noship_s_hit(int x, int y)
{
  set_noship(x-1, y-1);
  set_noship(x-1, y+1);
  set_noship(x+1, y-1);
  set_noship(x+1, y+1);
  set_noship(x, y-1);
  set_noship(x, y+1);
  set_noship(x-1, y);
  set_noship(x+1, y);
}

void set_noship_d_hit(int x, int y)
{
  set_noship(x-1, y-1);
  set_noship(x-1, y+1);
  set_noship(x+1, y-1);
  set_noship(x+1, y+1);
}

void set_noship_c_hit(int x, int y)
{
  set_noship(x-1, y-1);
  set_noship(x-1, y+1);
  set_noship(x+1, y-1);
  set_noship(x+1, y+1);
}

void set_noship_b_hit(int x, int y)
{
  set_noship(x-1, y-1);
  set_noship(x-1, y+1);
  set_noship(x+1, y-1);
  set_noship(x+1, y+1);
}
int b_count=0;
int c_count=0;
int d_count=0;

int X1=0;
int Y1=0;
int X2=0;
int Y2=0;
int X3=0;
int Y3=0;


  
void record_result(int x,int y,char line[])
{
  if(line[13]=='B')
  {
    //====kokokara====
    enemy_board[x][y] = BSHIP;
    b_count++;
    set_noship_b_hit(x,y);
    if(b_count%4==0){
      flag=0;
      set_noship_s_hit(x,y);
      set_noship_s_hit(X1,Y1);
      set_noship_s_hit(X2,Y2);
      set_noship_s_hit(X3,Y3);
    }
    else if(b_count%4==1){
      flag=1;
      X1=x;
      Y1=y;
      if(1<=x&& enemy_board[x-1][y]== UNKNOWN){
	enemy_board[x-1][y] = TEMP;
      }
      if(x<=7&&enemy_board[x+1][y]== UNKNOWN){
	enemy_board[x+1][y] = TEMP;
      }
      if(1<=y&&enemy_board[x][y-1]== UNKNOWN){
        enemy_board[x][y-1] = TEMP;
      }
      if(y<=7&&enemy_board[x][y+1]== UNKNOWN){
        enemy_board[x][y+1] = TEMP;
      }
    }
    else if(b_count%4==2){
      flag=1;
      X2=x;
      Y2=y;
      if(X2==X1&&max(Y1,Y2)<=7&&enemy_board[X2][max(Y1,Y2)+1]== UNKNOWN){
	enemy_board[X2][max(Y1,Y2)+1] = TEMP;
      }
      if(X2==X1&&min(Y1,Y2)>=1&&enemy_board[X2][min(Y1,Y2)-1]== UNKNOWN){
	enemy_board[X2][min(Y1,Y2)-1] = TEMP;
      }
      if(Y2==Y1&&max(X1,X2)<=7&&enemy_board[max(X1,X2)+1][Y2]== UNKNOWN){
	enemy_board[max(X1,X2)+1][Y2] = TEMP;
      }
      if(Y2==Y1&&min(X1,X2)>=1&&enemy_board[min(X1,X2)-1][Y2]== UNKNOWN){
	enemy_board[min(X1,X2)-1][Y2] = TEMP;
      }
    }
    else{
      flag=1;
      X3=x;
      Y3=y;
      if(X2==X1&&max(Y1,max(Y3,Y2))<=7&&enemy_board[X2][max(Y1,max(Y3,Y2))+1]== UNKNOWN){
	enemy_board[X2][max(Y1,max(Y3,Y2))+1] = TEMP;
      }
      if(X2==X1&&min(Y1,min(Y3,Y2))>=1&&enemy_board[X2][min(Y1,min(Y3,Y2))-1]== UNKNOWN){
	enemy_board[X2][min(Y1,min(Y3,Y2))-1] = TEMP;
      }
      if(Y2==Y1&&max(X1,max(X3,X2))<=7&&enemy_board[max(X1,max(X3,X2))+1][Y2]== UNKNOWN){
	enemy_board[max(X1,max(X3,X2))+1][Y2] = TEMP;
      }
      if(Y2==Y1&&min(X1,min(X3,X2))>=1&&enemy_board[min(X1,min(X3,X2))-1][Y2]== UNKNOWN){
	enemy_board[min(X1,min(X3,X2))-1][Y2] = TEMP;
      }
      
    }
      
      
	
    
    //====kokomade====
  }
  else if(line[13]=='C')
  {
    //====kokokara====
    enemy_board[x][y] = CSHIP;
    c_count++;
    set_noship_c_hit(x,y);
    if(c_count%3==0){
      flag=0;
      set_noship_s_hit(x,y);
      set_noship_s_hit(X1,Y1);
      set_noship_s_hit(X2,Y2);
    }
    else if(c_count%3==1){
      flag=1;
      X1=x;
      Y1=y;
      if(1<=x&& enemy_board[x-1][y]== UNKNOWN){
	enemy_board[x-1][y] = TEMP;
      }
      if(x<=7&&enemy_board[x+1][y]== UNKNOWN){
	enemy_board[x+1][y] = TEMP;
      }
      if(1<=y&&enemy_board[x][y-1]== UNKNOWN){
        enemy_board[x][y-1] = TEMP;
      }
      if(y<=7&&enemy_board[x][y+1]== UNKNOWN){
        enemy_board[x][y+1] = TEMP;
      }
    }
    else{
      flag=1;
      X2=x;
      Y2=y;
      if(X2==X1&&max(Y1,Y2)<=7&&enemy_board[X2][max(Y1,Y2)+1]== UNKNOWN){
	enemy_board[X2][max(Y1,Y2)+1] = TEMP;
      }
      if(X2==X1&&min(Y1,Y2)>=1&&enemy_board[X2][min(Y1,Y2)-1]== UNKNOWN){
	enemy_board[X2][min(Y1,Y2)-1] = TEMP;
      }
      if(Y2==Y1&&max(X1,X2)<=7&&enemy_board[max(X1,X2)+1][Y2]== UNKNOWN){
	enemy_board[max(X1,X2)+1][Y2] = TEMP;
      }
      if(Y2==Y1&&min(X1,X2)>=1&&enemy_board[min(X1,X2)-1][Y2]== UNKNOWN){
	enemy_board[min(X1,X2)-1][Y2] = TEMP;
      }
    }
	
      
    //====kokomade====
  }
  else if(line[13]=='D')
  {
    //====kokokara====

     enemy_board[x][y] = DSHIP;
     d_count++;
     set_noship_d_hit(x,y);
     if(d_count%2==0){
       flag=0;
       set_noship_s_hit(x,y);
       set_noship_s_hit(X1,Y1);
     }
     else{
       flag=1;
       X1=x;
       Y1=y;
       if(1<=x&& enemy_board[x-1][y]== UNKNOWN){
	 enemy_board[x-1][y] = TEMP;
       }
       if(x<=7&&enemy_board[x+1][y]== UNKNOWN){
	 enemy_board[x+1][y] = TEMP;
       }
       if(1<=y&&enemy_board[x][y-1]== UNKNOWN){
	 enemy_board[x][y-1] = TEMP;
       }
       if(y<=7&&enemy_board[x][y+1]== UNKNOWN){
	 enemy_board[x][y+1] = TEMP;
       }
       
     }
       
       
   
    //====kokomade====
  }
  else if(line[13]=='S')
  {
    //====kokokara====
     enemy_board[x][y] = SSHIP;
     set_noship_s_hit(x,y);
     
     
    //====kokomade====
  }
  else if(line[13]=='R')
  {
    //====kokokara====
    enemy_board[x][y] = ROCK;
    //====kokomade====
  }
  else
  {
    //====kokokara====
    enemy_board[x][y] = NOSHIP;

    //====kokomade====
  }
}
void print_board(void){
  int ix, iy;

  for (iy = BD_SIZE - 1; iy >= 0; iy--)
  {
    printf("%2d ", iy);
    for (ix = 0; ix < BD_SIZE; ix++)
    {
      switch(enemy_board[ix][iy])
      {
        case UNKNOWN:
          printf("U ");
          break;
        case NOSHIP:
          printf("N ");
          break;
        case ROCK:
          printf("R ");
          break;
        case BSHIP:
          printf("B ");
          break;
        case CSHIP:
          printf("C ");
          break;
        case DSHIP:
          printf("D ");
          break;
        case SSHIP:
          printf("S ");
          break;
        default:
          break;
      }
    }
    printf("\n");
  }

  printf("  ");
  for (ix = 0; ix < BD_SIZE; ix++)
  {
    printf("%2d", ix);
  }
  printf("\n\n");
}

void handle_messages(void)
{
  char line[MSG_LEN];

  srand(getpid());
  init_board();
  
  while (TRUE)
  {
    receive_from_ref(line);

    if(message_has_type(line, "name?"))
    {
      respond_with_name(); 
    }
    else if(message_has_type(line, "deployment?"))
    {
       respond_with_deployment(); 
    }
    else if(message_has_type(line, "shot?"))
    {
      respond_with_shot(); 
    }
    else if(message_has_type(line, "shot-result:"))
    {
      record_result(cur_x,cur_y,line);
      printf("[%s] result: %c\n", myName, line[13]);
      print_board();
    }
    else if(message_has_type(line, "end:"))
    {
      break;
    }
    else
    {
      printf("[%s] ignoring message: %s", myName, line);
    }
  }
}

int main()
{
  client_make_connection();
  handle_messages();
  client_close_connection();
  return 0;
}
