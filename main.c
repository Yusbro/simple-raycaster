#include <stdio.h>
#include <SDL2/SDL.h>

#define CELL_SIZE 32
#define PI 3.14152

typedef struct player{
	float px,py,pa;
}Player;


typedef struct KeyInput{
	int w,a,s,d;
}KeyInput;

//for managing the input!!!
KeyInput input_manager(KeyInput input, int up_or_down, int scan_code){
	KeyInput temp = input;
	switch(scan_code){
		case 26://W
			temp.w = up_or_down;	
			break;
		case 4://A
			temp.a = up_or_down;
			break;
		case 22://S
			temp.s = up_or_down;
			break;
		case 7://D
			temp.d = up_or_down;
			break;

		default:
			break;
	}

	return temp;
}



//for moving the player!!!

Player move_player(Player player, KeyInput input){
	Player moved_player = player;
	
	if(input.w){
		moved_player.px += cos(player.pa)*((input.w==1)+(-1*(input.s==1)));
		moved_player.py += sin(player.pa)*((input.w==1)+(-1*(input.s==1)));
	}
	
	if(input.a || input.d){
		moved_player.pa += (input.d==1)? 0.05: -0.05;
		if(moved_player.pa > 2*PI) moved_player.pa -= 2*PI;
		if(moved_player.pa < 0) moved_player.pa += 2*PI;
	}

	return moved_player;
}



//-----------------for drawing!!!
//completely for drawing the player!!!
static inline void draw_player(SDL_Renderer* render, Player player){
	SDL_SetRenderDrawColor(render, 100,50,200, 255);
	
	SDL_Rect rect = {player.px-3, player.py-3,6,6};
	SDL_RenderDrawRect(render, &rect);
	SDL_RenderDrawLine(render, player.px, player.py, player.px+cos(player.pa)*10, player.py+sin(player.pa)*10);
}


//just for debugging purposes
static inline void draw_grid(SDL_Renderer* render, int* map){	
	for(int y=0;y<8;y++){
		for(int x=0;x<8;x++){
			int index = x+y*8;
			
			//drawing the grid
			if(map[index]==0)SDL_SetRenderDrawColor(render, 0,255,0,255);
			if(map[index]==1)SDL_SetRenderDrawColor(render,255,0,0,255);
			SDL_Rect rect = {x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE};
			SDL_RenderDrawRect(render, &rect);
		}
	}
}


static inline int world_to_map(float f){
	return (int)floor(f/CELL_SIZE);
}

static inline float distance(float x1, float y1, float x2, float y2){
	return sqrt(((x1-x2)*(x1-x2))+  ( (y1-y2)*(y1-y2) )  );
}

static inline float deg_to_rad(float deg){
	if(deg==0) return 0;
	return deg * (PI/180.0);
}

static inline float clamp(float val, float min, float max){
	if(val>max) return max;
	if(val<min) return min;
	return val;
}

static inline void draw_ray(SDL_Renderer* render, Player player, int* map){
	float hxns=0, hyns=0, hrtx=0, hrty=0, hxs=0, hys=0;//, hang=0;
	float vxns=0, vyns=0, vrtx=0, vrty=0, vxs=0, vys=0;
	
	float ptmx = floorf(player.px/CELL_SIZE)*CELL_SIZE;
	float ptmy = floorf(player.py/CELL_SIZE)*CELL_SIZE;

	float ray_count = 100;
	float fov = 30;

	float player_angle = player.pa;
	float x = player.px, y = player.py;
	
	SDL_SetRenderDrawColor(render, 0,255,100, 255);

	for(int i=0;i<ray_count;i++){
		
		//SDL_RenderDrawLine(render, x,y, x+cos(angle)*500, y+sin(angle)*500);
		float angle = (player_angle-deg_to_rad(fov/2))+deg_to_rad((i/ray_count)*fov);
		//for horizontal stuff!!!
		if(sin(angle)<0){//for looking up
			hyns = y - ptmy + 0.1;
			hxns = hyns/tan(angle);
		}
		if(sin(angle)>0){//for looking down
			hyns = y - ptmy - CELL_SIZE ;
			hxns = hyns/tan(angle);
		}
		hrtx = x - hxns;hrty = y - hyns;
		hxs = hxns;
		hys = hyns;

		//for the vertical stuff!!
		if(cos(angle)>0){
			vxns = CELL_SIZE - (x - ptmx);
			vyns = vxns*tan(angle);
			vrtx = x + vxns;
			vrty = y + vyns;
		}
		if(cos(angle)<0){
			vxns = ( x - ptmx) + 0.1;
			vyns = vxns*tan(angle);
			vrtx = x - vxns;
			vrty = y - vyns;
		}
		vxs = vxns; 
		vys = vyns;
		 

		//do the horizontal and vertical ray check!!!!
		for(int ray_step=0;ray_step<8;ray_step++){
			
			//checking if da ray kinda collides with the stuff!!!
			int vhit=0, hhit=0;
			if(hrtx!=INFINITY){
				int hmx = world_to_map(hrtx), hmy = world_to_map(hrty);
				int h_index = hmx + hmy * 8;
				if(h_index < 64){
					if(map[h_index]!=0){
						hhit = 1;

					//	SDL_Rect rect = {hmx*CELL_SIZE, hmy*CELL_SIZE, CELL_SIZE, CELL_SIZE};
					//	SDL_RenderFillRect(render, &rect);
					}
				}
			}
			if(vrtx!=INFINITY){
				int vmx = world_to_map(vrtx), vmy = world_to_map(vrty);
				int v_index = vmx + vmy * 8;
				if(v_index < 64){
					if(map[v_index]!=0){
						vhit = 1;
					//	SDL_Rect rect = {vmx*CELL_SIZE, vmy*CELL_SIZE, CELL_SIZE, CELL_SIZE};
					//	SDL_RenderFillRect(render, &rect);
					}	
				}
			}
			



			//just doing the math for the horizontal stuff for now!!!
			//the horizontal part!!
			if(hhit==0){
				hys = CELL_SIZE;
				hxs = hys/tan(angle);	
				if(sin(angle)<0){//when looking up
					hrtx -= hxs;
					hrty -= hys + 0.1;
				}
				else{//when looking down!!
					hrtx += hxs;
					hrty += hys;
				}
			}

			//the vertical part!!!
			if(vhit==0){
				vxs = CELL_SIZE;
				vys = vxs*tan(angle);
				if(cos(angle)>0){
					vrtx += vxs;
					vrty += vys;
				}
				if(cos(angle)<0){
					vrtx -= vxs + 0.1;
					vrty -= vys;
				}
			}
		}//the loop ends here!!!
		
		//if got hit, find the closest stuff!!!
		float hdis = distance(hrtx, hrty, x,y);
		float vdis = distance(vrtx, vrty, x,y);
		
		float hpx=vrtx, hpy=vrty;
		int red = 0;
		if(vdis>hdis){
			hpx = hrtx; hpy = hrty;
			red = 255;
		}
		float ca = cos(angle - player_angle);	
		float dist = 1-( (distance(hpx, hpy, x, y)*ca)/ (CELL_SIZE*8));
		dist = clamp(dist, 0.01, 1);
		SDL_SetRenderDrawColor(render, red,255*(1-dist),255*dist,255);
		dist*= 400;
		
		SDL_Rect rect = {i*6, 200 - dist/2, 6,dist};
		SDL_RenderFillRect(render, &rect);

	}
}




int main(){
	int map[]={
		1,1,1,1,1,1,1,1,
		1,1,1,0,0,0,1,1,
		1,1,0,0,0,0,1,1,
		1,1,0,0,0,0,1,1,
		1,1,0,0,0,0,0,1,
		1,1,1,1,0,0,0,1,
		1,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,1
	};
	
	Player player={4*CELL_SIZE,4*CELL_SIZE,0};
	//the player control here!!
	KeyInput key_input = {0,0,0,0};
		


	//SDL stuff below here!!!
	SDL_Init(0);
	
	SDL_Window* window;
	SDL_Renderer* render;

	SDL_CreateWindowAndRenderer(600,400, 0, &window, &render);
	SDL_Event event;

	int running = 1;
	while(running){
		//taking care of the events
		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_KEYDOWN:
					key_input = input_manager(key_input, 1, event.key.keysym.scancode);
					break;
				case SDL_KEYUP:
					key_input = input_manager(key_input, 0, event.key.keysym.scancode);		
					break;
				
				case SDL_QUIT:
					running=0;
					break;

				default:
					break;

			}
		}
			
		player = move_player(player, key_input);

		//clearing the background stuff
		SDL_SetRenderDrawColor(render,0,0,0,255);
		SDL_RenderClear(render);
		
	//	draw_grid(render,&map[0]);
	//	draw_player(render, player);
		draw_ray(render, player, &map[0]);


		SDL_RenderPresent(render);
		SDL_Delay(1000/60);
	}
	SDL_Quit();
	return 0;
}
