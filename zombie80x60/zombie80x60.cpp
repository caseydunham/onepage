#include <windows.h>
#include <time.h>
#include <vector>
using namespace std;struct e{int x;int y;int dx;int dy;int state;};HANDLE wh;
HANDLE rh;CHAR_INFO bb[4000];COORD bs={80,50};COORD pos={0,0};e player;
SMALL_RECT a={0,0,79,49};COORD arena={25,15};int zombies=10;vector<e>z;bool 
ll,mt=false;int score,lives,level,leaps,i,j;void init(){wh=GetStdHandle(-11);rh
=GetStdHandle(-10);SMALL_RECT w={0,0,79,49};SetConsoleWindowInfo(wh,1,&w);COORD
b={80,50};SetConsoleScreenBufferSize(wh,b);}void setchar(int x,int y,char c,int 
a){bb[(y*80)+x].Char.AsciiChar=c;bb[(y*80)+x].Attributes=a;}void setpos(int x,
int y){COORD c1={x,y};SetConsoleCursorPosition(wh,c1);}void placeplayer(){int 
ok=0;while(!ok){player.x=(rand()%24)+1;player.y=(rand()%14)+1;for(i=0;i<z.size
();++i)if(z[i].state&&player.x!=z[i].x&&player.y!=z[i].y)ok=1;}}void input(){
DWORD e,er=0;GetNumberOfConsoleInputEvents(rh,&e);if(e!=0){INPUT_RECORD *r=new 
INPUT_RECORD[e];ReadConsoleInput(rh,r,e,&er);for(i=0;i<er;++i){if (r[i].
EventType==KEY_EVENT){WORD kc=r[i].Event.KeyEvent.wVirtualKeyCode;bool kd= 
r[i].Event.KeyEvent.bKeyDown;if(kd){if(kc==0x1B)exit(0);}}if(r[i].EventType==0x2
){if(r[i].Event.MouseEvent.dwButtonState&0x1){COORD pos=r[i].Event.MouseEvent.
dwMousePosition;if((pos.X>=0&&pos.X<arena.X)&&(pos.Y>=0&&pos.Y<arena.Y)){if(abs
(player.x-pos.X)<=1&&abs(player.y-pos.Y)<=1){player.x=pos.X;player.y=pos.Y;mt=
true;}}}if(r[i].Event.MouseEvent.dwButtonState&0x2){if(leaps-1>=0){--leaps;
placeplayer();}}}}delete[]r;}}void reset(){player.x=(rand()%24)+1;player.y=
(rand()%14)+1;player.dx=player.dy=player.state=1;z.clear();for(i=0;i<zombies;++
i){while(1){e t={(rand()%24)+1,(rand()%14)+1,1,1,1};if(t.x!=player.x&&t.y!=
player.y){z.push_back(t);break;}}}}void draw(){for(i=0;i<arena.Y;++i)for(j=0;j
<arena.X; ++j)setchar(j,i,219,2);setchar(player.x,player.y,1,14);for(i=0;i<z.
size(); ++i)if(z[i].state)setchar(z[i].x,z[i].y,'Z',4);WriteConsoleOutput(wh,bb,
bs,pos,&a);setpos(30,0);printf("Zombie - casey.dunham@gmail.com");setpos(30,1);
printf("Score: %d",score);setpos(30,2);printf("Lives: %d",lives);setpos(30,3);
printf("Leaps: %d",leaps);setpos(30,4);printf("Level: %d",level);}void update() 
{if(mt){for(i=0;i<z.size();++i){if(z[i].state){int x=player.x-z[i].x;int y= 
player.y-z[i].y;if(x<0)x=-1;else if(x>0)x=1;if(y<0)y=-1;else if(y>0)y=1;
z[i].x+=x;z[i].y+=y;if(z[i].x==player.x&&z[i].y==player.y){--zombies;ll=true;
return;}for(j=i+1;j<z.size();++j){if(z[j].state&&(z[i].x==z[j].x)&&(z[i].y==z[j
].y)){zombies-=2;z[i].state=z[j].state=0;score+=15;}}}}mt=false;}}void main(){
score=0;level=1;lives=4;leaps=3;srand(time(0));init();reset();while(1){memset(
bb,4000*sizeof(CHAR_INFO),0);input();update();draw();if(!lives){setpos(35,25);
printf("GAME OVER");Sleep(5000);reset();}else if(!z.size()){++level;reset();}
else if(ll){Sleep(1000);placeplayer();--lives;player.state=1;reset();ll=false;}
else Sleep(50);}}
