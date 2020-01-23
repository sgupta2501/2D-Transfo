/* Name: Samarth Gupta
Class XII-D
Roll No.:22

Project: C program to draw 2D objects and to demonstrate 2D transformations:
	-Translation
	-Scaling
	-Rotation about an arbitrary point (a,b) by angle theta
	-Reflection about an arbitrary line ax+by+c=0
*/

#include<stdio.h>
#include<graphics.h>
#include<conio.h>
#include <iostream.h>
#include<dos.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<fstream.h>
#include<iomanip.h>

//default parameters
#define dlytime 1000 //1 millisec=1000
#define snderr 100
#define sndval 200 //15 to 3000
#define arrSz 100
#define maxVertex 20

struct 	pixel{
	int x;//x coordinate of a point on the screen
	int y;//y coordinte
};

//structure for storing the object characteristics
struct obj_def{
	int noVertex;//to store no of vertices in the object
	int startangle;
	int endangle; // default values 0,360;
	int closedpoly;//by default polygon is closed
	int maxx;
	int maxy; //dt.getobj().maxx and dt.getobj().maxy for graphics screen
	int usermaxx;
	int usermaxy;//for my laptop user x-axis: -319 to 319; y-axis: -239 to 239
	pixel xy[maxVertex];//to store the vertex coordinates
};

//structure for storing transformations on an object
struct transfo_def{
	char type;//type of transformation: t:translate, s:scaling, o:rotation, r:reflection

	union param_def{ //parameters for transformation
		float trSc[2];//translation, scaling
		float rot[3];//rotation
		int ref[3];//reflection
	}param;
};

//default GUI options
struct gui_def{
	int color;//1 blue, 0 black
	int coordcolor;//4 red, 0 black
	int bkcolor;//black, 15 white
	int linestyle;//solid line//1 dotted line
	int linewidth;//norm width//3 thick width
	int fillstyle;//, SOLID_FILL
	int fillcolor;//green
	int textfont; //0 default font,10;//BOLD_FONT
	int textdir;//default is horiz dir
	int usrpattern;//for line
	int charsize; //1:normal, 2:large, 0:user defined
};


class drawTransform{
	gui_def gui;
	obj_def obj;
public:
	transfo_def arrTransfo;

	void setGUI();
	pixel findScreenCoord(pixel xy);
	pixel findUserCoord(pixel screenXY);
	void drawCoord ();
	void mycleardevice();
	void getObjCoord ();
	void drawObj();
	void translate(int mvX=0, int mvY=0);
	void scale(float mvX=1, float mvY=1);
	void rotate(pixel pix, float mvX);
	void reflect_Yaxis();
	void reflect(int a, int b, int c);
	char transfo(int wait=1);
	void getGUI(int pref, ifstream &fin);
	void setObjDefault();

	gui_def& getgui()
	{
		return gui;
	}
	obj_def& getobj()
	{
		return obj;
	}
};

//set GUI parameters like color, fill style, font
void drawTransform::setGUI()
{
	setcolor(gui.color);
	setbkcolor(gui.bkcolor);
	setlinestyle(gui.linestyle, gui.usrpattern, gui.linewidth);
	setfillstyle(gui.fillstyle, gui.fillcolor);
}

//function takes the coordinates from user
//returns corresponding screen coordinates
pixel drawTransform::findScreenCoord(pixel xy)
{
	pixel screenXY;
	screenXY.x = xy.x + obj.usermaxx;
	screenXY.y = obj.usermaxy - xy.y;

	return screenXY;
}

pixel drawTransform::findUserCoord(pixel screenXY)
{
	pixel xy;
	xy.x = screenXY.x - obj.usermaxx;
	xy.y = obj.usermaxy - screenXY.y;

	return xy;
}

//draw the coorinate axis on the screen
void drawTransform::drawCoord ()
{
	char str[50];

	//set GUI parameters for coordinate axis
	setcolor(gui.coordcolor);
	settextstyle(gui.textfont,gui.textdir,gui.charsize);

	line(obj.usermaxx, 0, obj.usermaxx, obj.maxy);
	line(0, obj.usermaxy, obj.maxx, obj.usermaxy);
	//writing the coordinates
	outtextxy(obj.usermaxx+1,obj.usermaxy+1,"(0,0)");
	sprintf(str,"(%d,0)",obj.usermaxx);
	outtextxy(obj.maxx-50-strlen(str),obj.usermaxy+1,str);
	sprintf(str,"(-%d,0)",obj.usermaxx);
	outtextxy(2,obj.usermaxy+1,str);
	sprintf(str,"(0,%d)",obj.usermaxy);
	outtextxy(obj.usermaxx+1,1,str);
	sprintf(str,"(0,-%d)",obj.usermaxy);
	outtextxy(obj.usermaxx+1,obj.maxy-8,str);
	//menu  for transformation
	outtextxy(540,2,"1: Translate");
	outtextxy(540,12,"2: Scaling");
	outtextxy(540,24,"3: Rotate");
	outtextxy(540,36,"4: Reflect");
	outtextxy(540,48,"5: Exit");
}


/*function to always have the coordinate axis on the screen while clearing the screen*/
void drawTransform::mycleardevice()
{
	cleardevice();
	drawCoord ();
}

//get the coordinates for the vertices
void drawTransform::getObjCoord ()
{
	//taking parameters to draw an object
	cout<<"\n\n\n                         Object Coordinates\n\n";
	cout<<"**************************************************************\n\n";
	cout<<"Enter the number of object vertices (0-20) (0: circle/ellipse): ";
	cin>>obj.noVertex;
	if (obj.noVertex<0 || obj.noVertex>maxVertex)
	{
		cout<<"\n\nError:Invalid value for number of vertices!\nExiting ";
		sound(snderr);
		for (int i=0; i<10; i++)
		{
			delay(dlytime/10);
			cout<<"..";
		}
		nosound();
		exit(-1);
	}
	cout<<"\n\nx-axis: -"<<obj.usermaxx<<" to "<<obj.usermaxx<<" y-axis: -"<<obj.usermaxy<<" to "<<obj.usermaxy<<endl;
	if (obj.noVertex==0)
	{
		cout<<"\nEnter x and y for center: ";
		cin>>obj.xy[0].x>>obj.xy[0].y;
		cout<<"\nEnter x-radius, y-radius: ";
		cin>>obj.xy[1].x>>obj.xy[1].y;
		cout<<"\nEnter start angle, end angle (positive values): ";
		cin>>obj.startangle>>obj.endangle;
		cout<<endl;
	}
	else
	{
		if (obj.noVertex>2)//polygon
		{
			cout<<"\nIs polygon closed/open (1/0): ";
			cin>>obj.closedpoly;
			cout<<endl;
		}
	}
	for (int i=0; i<obj.noVertex; i++)
	{
		cout<<"\nEnter x and y for vertex "<<i+1<<" :";
		cin>>obj.xy[i].x>>obj.xy[i].y;
		cout<<endl;
	}
}

//draw the object on the screen
void drawTransform::drawObj()
{
	int i;
	// Draw object
	char str[20];

	//reset GUI parameters for drawing
	setGUI();

	pixel pix=findUserCoord(obj.xy[0]);
	sprintf(str,"(%d,%d)",pix.x,pix.y);
	outtextxy(obj.xy[0].x+5,obj.xy[0].y+5,str);
	if (obj.noVertex==0)
	{
		ellipse(obj.xy[0].x,obj.xy[0].y,obj.startangle,obj.endangle,obj.xy[1].x,obj.xy[1].y);
		if ((abs((obj.endangle-obj.startangle)%360)==0)&&(gui.fillstyle!=EMPTY_FILL))
			floodfill(obj.xy[0].x,obj.xy[0].y,gui.color);//3rd arg gives boundary color
		outtextxy(obj.xy[0].x,obj.xy[0].y,".");
	}
	else if (obj.noVertex==1)
	{
		outtextxy(obj.xy[0].x,obj.xy[0].y,".");

	}
	else if (obj.noVertex==2) //line
	{
		line(obj.xy[0].x, obj.xy[0].y, obj.xy[1].x, obj.xy[1].y);
		pix=findUserCoord(obj.xy[1]);
		sprintf(str,"(%d,%d)",pix.x,pix.y);
		outtextxy(obj.xy[1].x+5,obj.xy[1].y+5,str);
	}
	else //polygon
	{
		if (obj.closedpoly==0)//open polygon to be drawn
		{
			//using drawpoly as it does not auto close the polygon
			//make array for using fillpoly functions
			int poly[40];//max 20 vertices

			for (i=0; i<obj.noVertex; i++)
			{
				poly[2*i]=obj.xy[i].x;
				poly[2*i+1]=obj.xy[i].y;
			}
			drawpoly(obj.noVertex,poly);
		}
		else //closed polygon is to be drawn
		{
			//not using drawpoly as it does not auto close the polygon
			if (gui.fillstyle==EMPTY_FILL)
			{
				for (i=0; i<obj.noVertex; i++)
				{
					line(obj.xy[i].x, obj.xy[i].y, obj.xy[(i+1)%obj.noVertex].x, obj.xy[(i+1)%obj.noVertex].y);
				}
			}
			else //filled polygon
			{
				//make array for using fillpoly functions
				int poly[40];//max 20 vertices

				for (i=0; i<obj.noVertex; i++)
				{
					poly[2*i]=obj.xy[i].x;
					poly[2*i+1]=obj.xy[i].y;
				}
				fillpoly(obj.noVertex,poly);
			}
		}
		//print vertex coordinates
		for (i=1; i<obj.noVertex; i++)
		{
			pix=findUserCoord(obj.xy[i]);
			sprintf(str,"(%d,%d)",pix.x,pix.y);
			outtextxy(obj.xy[i].x,obj.xy[i].y+5,str);
		}
	}
}

//function to translate the object
void drawTransform::translate(int mvX, int mvY)
{
	//mvX=amount of translation in direction of x-axis
	//mvY=amount of translation in direction of y-axis

	int i;

	if(obj.noVertex==0)
	{
		obj.xy[0].x+=mvX;
		obj.xy[0].y-=mvY;
	}
	else
	{
		for(i=0;i<obj.noVertex;i++)
		{
			obj.xy[i].x+=mvX;
			obj.xy[i].y-=mvY;
		}
	}
	mycleardevice();
	drawObj();
}

//function to scale a 2D object
void drawTransform::scale(float mvX, float mvY)
{

	//mvX=scale factor in direction of x-axis
	//mvY=scale factor in direction of y-axis

	int i;
	if(obj.noVertex==0)
	{
		obj.xy[0]=findUserCoord(obj.xy[0]);
		obj.xy[0].x*=mvX;
		obj.xy[0].y*=mvY;
		obj.xy[1].x*=mvX;
		obj.xy[1].y*=mvY;
		obj.xy[0]=findScreenCoord(obj.xy[0]);
	}
	else
	{
		for(i=0;i<obj.noVertex;i++)
		{
			obj.xy[i]=findUserCoord(obj.xy[i]);
			obj.xy[i].x*=mvX;
			obj.xy[i].y*=mvY;
			obj.xy[i]=findScreenCoord(obj.xy[i]);
		}
	}
	mycleardevice();
	drawObj();
}

void drawTransform::rotate(pixel pix, float mvX)
{
	int i;
	double angle=-1 * (M_PI/180)*mvX;//degree to radian
	double cosVal=cos(angle);
	double sinVal=sin(angle);
	double tmp, tmp1;
	if((pix.x!=0) || (pix.y!=0))
		translate(-1 *pix.x, -1 *pix.y);//is in screen coordinates
	if(obj.noVertex==0)
	{
		obj.xy[0]=findUserCoord(obj.xy[0]);
		tmp=(obj.xy[0].x * cosVal)+ (obj.xy[0].y * sinVal);
		tmp1=(-1*obj.xy[0].x * sinVal)+ (obj.xy[0].y * cosVal);
		int tmp2=(int)ceil(tmp), tmp3=(int)ceil(tmp1);
		//habdle rounding error while converting double to int
		if (tmp-tmp2 >0.5 || tmp2-tmp>0.5)
		{
			if (tmp<0 && tmp<tmp2)
				tmp2--;
			else if (tmp<0 && tmp>tmp2)
				tmp2++;
			else if (tmp>0 && tmp<tmp2)
				tmp2--;
			else if (tmp>0 && tmp>tmp2)
				tmp2++;
		}
		if (tmp1-tmp3 >0.5 || tmp3-tmp1>0.5)
		{
			if (tmp1<0 && tmp1<tmp3)
				tmp3--;
			else if (tmp1<0 && tmp1>tmp3)
				tmp3++;
			else if (tmp1>0 && tmp1<tmp3)
				tmp3--;
			else if (tmp1>0 && tmp1>tmp3)
				tmp3++;
		}
		obj.xy[0].x = tmp2;
		obj.xy[0].y= tmp3	;
		obj.xy[0]=findScreenCoord(obj.xy[0]);
		obj.startangle+= mvX;
		obj.endangle+= mvX;
	}
	else
	{
		for(i=0;i<obj.noVertex;i++)
		{
			obj.xy[i]=findUserCoord(obj.xy[i]);
			int xVal=obj.xy[i].x, yVal=obj.xy[i].y;
			tmp=(xVal*cosVal)+(yVal*sinVal);
			tmp1=(-1*xVal*sinVal)+(yVal*cosVal);
			int tmp2=(int)ceil(tmp), tmp3=(int)ceil(tmp1);
			if (tmp-tmp2 >0.5 || tmp2-tmp>0.5)
			{
				if (tmp<0 && tmp<tmp2)
					tmp2--;
				else if (tmp<0 && tmp>tmp2)
					tmp2++;
				else if (tmp>0 && tmp<tmp2)
					tmp2--;
				else if (tmp>0 && tmp>tmp2)
					tmp2++;
			}
			if (tmp1-tmp3 >0.5 || tmp3-tmp1>0.5)
			{
				if (tmp1<0 && tmp1<tmp3)
					tmp3--;
				else if (tmp1<0 && tmp1>tmp3)
					tmp3++;
				else if (tmp1>0 && tmp1<tmp3)
					tmp3--;
				else if (tmp1>0 && tmp1>tmp3)
					tmp3++;
			}
			obj.xy[i].x = tmp2;
			obj.xy[i].y= tmp3;
			obj.xy[i]=findScreenCoord(obj.xy[i]);
		}
	}
	if((pix.x!=0) || (pix.y!=0))
		translate(pix.x, pix.y);
	mycleardevice();
	drawObj();
}


void drawTransform::reflect_Yaxis()
{
	int i;
	if(obj.noVertex==0)
	{
		obj.xy[0]=findUserCoord(obj.xy[0]);
		obj.xy[0].x=(-1)*obj.xy[0].x;
		obj.xy[0]=findScreenCoord(obj.xy[0]);
	}
	else
		for(i=0;i<obj.noVertex;i++)
		{
			obj.xy[i]=findUserCoord(obj.xy[i]);
			obj.xy[i].x=(-1)*obj.xy[i].x;
			obj.xy[i]=findScreenCoord(obj.xy[i]);
		}
}


//general reflect function for reflection about line ax+by+c=0
void drawTransform::reflect(int a, int b, int c)
{
	int i;
	if (a==0 && b==0)
		return;
	if (b==0) //reflect about a line parallel to y-axis, ax+c=0
	{
		float intercept=(-1.0*c)/a;//intercept of the line about which reflecting
		translate(-1*intercept,0);
		reflect_Yaxis();
		translate(1*intercept,0);
	}
	else
	{
		float intercept=(-1.0*c)/b;//intercept of the line about which reflecting
		float m=-1*(a/b);//slope of the line about which reflecting
		float angle= (M_PI/2-atan(m))*(180/M_PI); //angle btw y-axis and line in degree
		pixel orig={0,0};
		//Step1:move the y-intercept to origin
		translate(0,-1*intercept);
		//Step2: rotate the line to coincide with y-axis
		rotate(orig, angle);
		//Step3: reflect about y-axis
		reflect_Yaxis();
		//Step4: reverse rotation
		rotate(orig, 360-angle);
		//Step5:reverse translation
		translate(0,intercept);
	}
	mycleardevice();
	drawObj();
}


//function that takes user choice of transformation as input
//works by calling appropriate function for transformation
char drawTransform::transfo(int wait)
{
	char kb,kb1,str[20],str1[20];
	float trX, trY; //variables for translation, scaling
	int a,b,c; //var1ables for reflection
	pixel pix;

	if (wait==1)
	{
		while (!kbhit()) /* do nothing */ ;
		sound(sndval);
		delay(dlytime/10);
		nosound();
		kb=getch();
	}
	else //no waiting, already have a transformation to perform
	{
		//set kb value acc to input transfo
		switch (arrTransfo.type)
			{
			case 't'://translation
				kb='1';
				break;
			case 's'://scaling
				kb='2';
				break;
			case 'o'://rotation
				kb='3';
				break;
			case 'r'://reflection
				kb='4';
				break;
			default:
				kb='5';
				break;
		}

	}
	switch (kb)
	{
		case '1': //translate
		if (wait==0)
		{
			trX=arrTransfo.param.trSc[0];
			trY=arrTransfo.param.trSc[1];
		}
		else
		{
			rectangle(500,60,630,110);
			outtextxy(505,65,"Translating...");
			outtextxy(505,80,"x-value= ");
			strcpy(str,"\0");
			do
			{
				while (!kbhit()) /* do nothing */ ;
				kb1=getch();
				if (kb1=='\r')
					break;
				sprintf(str1,"%c",kb1);
				strcat(str,str1);
				outtextxy(585,80,str);
			} while(1);
			trX = atoi(str);
			outtextxy(505,100,"y-value= ");
			strcpy(str,"\0");
			do
			{
				while (!kbhit()) /* do nothing */ ;
				kb1=getch();
				if (kb1=='\r')
					break;
				sprintf(str1,"%c",kb1);
				strcat(str,str1);
				outtextxy(585,100,str);
			} while(1);
			trY= atoi(str);

			//store the tranformation
			arrTransfo.type='t';
			arrTransfo.param.trSc[0]=trX;
			arrTransfo.param.trSc[1]=trY;
		}
		translate(trX, trY);
		break;
		case '2'://scale
		if (wait==0)
		{
			trX=arrTransfo.param.trSc[0];
			trY=arrTransfo.param.trSc[1];
		}
		else
		{
			rectangle(500,60,630,110);
			outtextxy(505,65,"Scaling...");
			outtextxy(505,80,"x-value= ");
			strcpy(str,"\0");
			do
			{
				while (!kbhit()) /* do nothing */ ;
				kb1=getch();
				if (kb1=='\r')
					break;
				sprintf(str1,"%c",kb1);
				strcat(str,str1);
				outtextxy(585,80,str);
			} while(1);
			trX = atof(str);
			outtextxy(505,100,"y-value= ");
			strcpy(str,"\0");
			do
			{
				while (!kbhit()) /* do nothing */ ;
				kb1=getch();
				if (kb1=='\r')
					break;
				sprintf(str1,"%c",kb1);
				strcat(str,str1);
				outtextxy(585,100,str);
			} while(1);
			trY= atof(str);

			//store the tranformation
			arrTransfo.type='s';
			arrTransfo.param.trSc[0]=trX;
			arrTransfo.param.trSc[1]=trY;
		}
		scale(trX, trY);
		break;
		case '3'://rotate
		if (wait==0)
		{
			pix.x=(int)arrTransfo.param.rot[0];
			pix.y=(int)arrTransfo.param.rot[1];
			trX=arrTransfo.param.rot[2];
		}
		else
		{
			rectangle(500,60,630,110);
			outtextxy(505,65,"Rotating...");
			outtextxy(505,80,"Angle(deg)= ");
			strcpy(str,"\0");
			do
			{
				while (!kbhit()) /* do nothing */ ;
				kb1=getch();
				if (kb1=='\r')
					break;
				sprintf(str1,"%c",kb1);
				strcat(str,str1);
				outtextxy(592,80,str);
			} while(1);
			trX = atof(str);
			outtextxy(505,100,"Point= ");
			strcpy(str,"\0");
			do
			{
				while (!kbhit()) /* do nothing */ ;
				kb1=getch();
				if (kb1=='\r')
					break;
				sprintf(str1,"%c",kb1);
				strcat(str,str1);
				outtextxy(585,100,str);
			} while(1);
			char *ptr=strstr(str," ");
			ptr++;
			int len=strlen(str),len1=strlen(ptr);
			str[len-len1-1]='\0';//ptr starts from space
			pix.x= atoi(str);
			pix.y= atoi(ptr);

			//store the tranformation
			arrTransfo.type='o';
			arrTransfo.param.rot[0]=pix.x;
			arrTransfo.param.rot[1]=pix.y;
			arrTransfo.param.rot[2]=trX;//degree to radian
		}
		rotate(pix, trX);
		break;
		case '4'://reflect
		if (wait==0)
		{
			a=arrTransfo.param.ref[0];
			b=arrTransfo.param.ref[1];
			c=arrTransfo.param.ref[2];
		}
		else
		{
			rectangle(500,60,632,140);
			outtextxy(505,65,"Reflecting about");
			outtextxy(505,80,"ax+by+c=0:");
			outtextxy(505,95,"a (int)= ");
			strcpy(str,"\0");
			do
			{
				while (!kbhit()) /* do nothing */ ;
				kb1=getch();
				if (kb1=='\r')
					break;
				sprintf(str1,"%c",kb1);
				strcat(str,str1);
				outtextxy(585,95,str);
			} while(1);
			a = atoi(str);
			outtextxy(505,110,"b (int)= ");
			strcpy(str,"\0");
			do
			{
				while (!kbhit()) /* do nothing */ ;
				kb1=getch();
				if (kb1=='\r')
					break;
				sprintf(str1,"%c",kb1);
				strcat(str,str1);
				outtextxy(585,110,str);
			} while(1);
			b= atoi(str);
			//printf("b=%d ",b);
			outtextxy(505,125,"c (int)= ");
			strcpy(str,"\0");
			do
			{
				while (!kbhit()) /* do nothing */ ;
				kb1=getch();
				if (kb1=='\r')
					break;
				sprintf(str1,"%c",kb1);
				strcat(str,str1);
				outtextxy(585,125,str);
			} while(1);
			c = atoi(str);

			//store the tranformation
			arrTransfo.type='r';
			arrTransfo.param.ref[0]=a;
			arrTransfo.param.ref[1]=b;
			arrTransfo.param.ref[2]=c;
		}
		reflect(a, b, c);
		break;
		default://exit loop
		//kb='5';
		break;
	}
 return kb;
}

//function to display menu of GUI options and to get the values
void drawTransform::getGUI(int pref, ifstream &fin)
{
	//set default gui values
	gui.color=1;//1 blue, 0 black
	gui.coordcolor=4;//4 red, 0 black
	gui.bkcolor=15;//black, 15 white
	gui.linestyle=DOTTED_LINE;//solid line//1 dotted line
	gui.linewidth=1;//norm width//3 thick width
	gui.fillstyle=EMPTY_FILL;//, SOLID_FILL
	gui.fillcolor=2;//green
	gui.textfont=0; //0 default font,10;//BOLD_FONT
	gui.textdir=HORIZ_DIR;//default is horiz dir
	gui.usrpattern=1;//for line
	gui.charsize=1; //1:normal, 2:large, 0:user defined

	if (pref==0) //take default GUI options 
	{
		//default values are used, nothing is done here
	}
	else if (pref==1) //take gui options from a stored file
	{
		//if fin is not defined, default parameters will get picked
		while(fin)
		{
			//fin.read((char*)&gui,sizeof(gui));
			fin>>obj.closedpoly;
			fin>>gui.color;
			fin>>gui.coordcolor;
			fin>>gui.bkcolor;
			fin>>gui.linestyle;
			fin>>gui.linewidth;
			fin>>gui.fillstyle;
			fin>>gui.fillcolor;
			fin>>gui.textfont;
			fin>>gui.textdir;
			fin>>gui.usrpattern;
			fin>>gui.charsize;
		}
	}
	else //from screen options
	{
		//function works in graphics mode
		int i,j;
		char *cname[]={"BLACK","BLUE","GREEN","CYAN","RED","MAGENTA","BROWN","LIGHT GRAY","DARK GRAY","LIGHT BLUE","LIGHT GREEN","LIGHT CYAN","LIGHT RED","LIGHT MAGENTA","YELLOW","WHITE"};
		char *lstyle[]={"SOLID LINE","DOTTED LINE","CENTER LINE","DASHED LINE","USERBIT LINE"};
		char *lwidth[]={"NORM WIDTH","THICK WIDTH"};//1,3pixels wide
		char *fname[] = {"EMPTY_FILL", "SOLID_FILL", "LINE_FILL", "LTSLASH_FILL", "SLASH_FILL", "BKSLASH_FILL", "LTBKSLASH_FILL", "HATCH_FILL", "XHATCH_FILL", "INTERLEAVE_FILL", "WIDE_DOT_FILL", "CLOSE_DOT_FILL", "USER_FILL" };
		/* the names of the text styles supported */
		char *fontname[] = {"DEFAULT","TRIPLEX","SMALL","SANS SERIF","GOTHIC","SCRIPT","SIMPLEX","TRIPLEX SCRIPT","COMPLEX","EUROPEAN","BOLD"};
		char str[20],str1[20],kb;

		//set color values for text/drawing, fill, coordinate axis, bkgnd
		rectangle(50,50,350,350);
		outtextxy(125,55,"Text Color Options");
		j=85;
		setcolor(WHITE);//default color setting to white
		for (i=BLACK; i<=WHITE; i++)
		{
			if (i!=0)
				setcolor(i);
			outtextxy(60,j+i*15,cname[i]);
			sprintf(str,"%d",i);
			outtextxy(250,j+i*15, str);
		}
		rectangle(380,50,600,350);
		outtextxy(400,55,"Please select options ");
		outtextxy(400,78,"Text/draw Color :");
		outtextxy(410,88,"(except black)");
		strcpy(str,"\0");
		do
		{
			while (!kbhit());
			kb=getch();
			if (kb=='\r')
				break;
			sprintf(str1,"%c",kb);
			strcat(str,str1);
			outtextxy(545,78,str);
		}while(1);
		gui.color=atoi(str);
		if (gui.color==0)
			gui.color=1;//black text/draw color does not work
		outtextxy(400,100,"Background Color: ");
		strcpy(str,"\0");
		do
		{
			while (!kbhit());
			kb=getch();
			if (kb=='\r')
				break;
			sprintf(str1,"%c",kb);
			strcat(str,str1);
			outtextxy(545,100,str);
		}while(1);
		gui.bkcolor=atoi(str);
		outtextxy(400,115,"Fill Color: ");
		strcpy(str,"\0");
		do
		{
			while (!kbhit());
			kb=getch();
			if (kb=='\r')
				break;
			sprintf(str1,"%c",kb);
			strcat(str,str1);
			outtextxy(545,115,str);
		}while(1);
		gui.fillcolor=atoi(str);
		outtextxy(400,130,"Coord. Axis Color: ");
		outtextxy(400,140,"(except black)");
		strcpy(str,"\0");
		do
		{
			while (!kbhit());
			kb=getch();
			if (kb=='\r')
				break;
			sprintf(str1,"%c",kb);
			strcat(str,str1);
			outtextxy(545,130,str);
		}while(1);
		gui.coordcolor=atoi(str);
		cleardevice();
		//set text font
		rectangle(50,50,350,350);
		outtextxy(125,55,"Text Font Options");
		j=85;
		setcolor(WHITE);//default color setting to white
		for (i=DEFAULT_FONT; i<=BOLD_FONT; i++)
		{
			outtextxy(60,j+i*15,fontname[i]);
			sprintf(str,"%d",i);
			outtextxy(250,j+i*15, str);
		}
		rectangle(380,50,600,350);
		outtextxy(400,60,"Please select options ");
		outtextxy(400,85,"Text Font: ");
		strcpy(str,"\0");
		do
		{
			while (!kbhit());
			kb=getch();
			if (kb=='\r')
				break;
			sprintf(str1,"%c",kb);
			strcat(str,str1);
			outtextxy(545,85,str);
		}while(1);
		gui.textfont=atoi(str);
		cleardevice();
		setcolor(WHITE);//default color setting to white
		rectangle(50,50,350,450);
		outtextxy(125,55,"Line/Fill Settings");
		setcolor(CYAN);//color for printing line style settings
		outtextxy(60,85,"Line Style:");
		j=100;
		for (i=SOLID_LINE; i<=USERBIT_LINE; i++)
		{
			outtextxy(60,j+i*15,lstyle[i]);
			sprintf(str,"%d",i);
			outtextxy(250,j+i*15, str);
		}
		setcolor(YELLOW);//color for printing line size settings
		outtextxy(60,180,"Line Size:");
		outtextxy(60,195, lwidth[0]);
		outtextxy(250,195, "1");
		outtextxy(60,210,lwidth[1]);
		outtextxy(250,210, "3");
		//fill settings
		setcolor(GREEN);//color for printing fill settings
		outtextxy(60,230,"Fill Style:");
		j=245;
		for (i=EMPTY_FILL; i<USER_FILL; i++)
		{
			outtextxy(60,j+i*15,fname[i]);
			sprintf(str,"%d",i);
			outtextxy(250,j+i*15, str);
		}
		setcolor(WHITE);//color for selecting settings
		rectangle(380,50,600,250);
		outtextxy(400,60,"Please select options ");
		outtextxy(400,85,"Line Style: ");
		strcpy(str,"\0");
		do
		{
			while (!kbhit());
			kb=getch();
			if (kb=='\r')
				break;
			sprintf(str1,"%c",kb);
			strcat(str,str1);
			outtextxy(540,85,str);
		}while(1);
		gui.linestyle=atoi(str);
		outtextxy(400,100,"Line Width: ");
		strcpy(str,"\0");
		do
		{
			while (!kbhit());
			kb=getch();
			if (kb=='\r')
				break;
			sprintf(str1,"%c",kb);
			strcat(str,str1);
			outtextxy(540,100,str);
		}while(1);
		gui.linewidth=atoi(str);
		outtextxy(400,115,"Fill Style: ");
		strcpy(str,"\0");
		do
		{
			while (!kbhit());
			kb=getch();
			if (kb=='\r')
				break;
			sprintf(str1,"%c",kb);
			strcat(str,str1);
			outtextxy(540,115,str);
		}while(1);
		gui.fillstyle=atoi(str);
		gui.usrpattern=1;//default
		if (gui.linestyle==USERBIT_LINE)
		{
			outtextxy(400,130,"User Pattern: ");
				strcpy(str,"\0");
			do
			{
				while (!kbhit());
				kb=getch();
				if (kb=='\r')
					break;
				sprintf(str1,"%c",kb);
				strcat(str,str1);
				outtextxy(540,130,str);
			}while(1);
			gui.usrpattern=atoi(str);
		}
		cleardevice();
	}
}

//set default values
void drawTransform::setObjDefault()
{
	obj.startangle=0;// default values 0,360;
	obj.endangle=360; // default values 0,360;
	obj.usermaxx=319;
	obj.usermaxy=239;//for my laptop user x-axis: -319 to 319; y-axis: -239 to 239
	obj.closedpoly=1;//by default polygon is closed
}

void fileInsert(ofstream &fi, drawTransform &dti, char modi)
{
	drawTransform dtnew;
	dtnew.setObjDefault();
	clrscr();
	dtnew.getObjCoord();//is in user coord
	//convert to screen before saving
	if(dtnew.getobj().noVertex==0)
		dtnew.getobj().xy[0]=dtnew.findScreenCoord(dtnew.getobj().xy[0]);
	else
		for(int i=0;i<dtnew.getobj().noVertex;i++)
			dtnew.getobj().xy[i]=dtnew.findScreenCoord(dtnew.getobj().xy[i]);
	clrscr();
	cout<<"\n\n\n                         Transformation\n\n";
	cout<<"**************************************************************\n\n";
	cout<<"Type: t:translation, s:scaling, o:rotation, r:reflection :";
	cin>>dtnew.arrTransfo.type;
	if ((dtnew.arrTransfo.type !='t')&&(dtnew.arrTransfo.type !='s')&&(dtnew.arrTransfo.type !='o')&&(dtnew.arrTransfo.type !='r'))
	{
		cout<<"\nIncorrect option, assuming translation\n";
		dtnew.arrTransfo.type='t';
	}
	switch (dtnew.arrTransfo.type)
	{
		case 't':
			cout<<"\nTranslation x-axis: ";
			cin>>dtnew.arrTransfo.param.trSc[0];
			cout<<"\nTranslation y-axis: ";
			cin>>dtnew.arrTransfo.param.trSc[1];
			break;
		case 's':
			cout<<"\nScaling x-axis: ";
			cin>>dtnew.arrTransfo.param.trSc[0];
			cout<<"\nScaling y-axis: ";
			cin>>dtnew.arrTransfo.param.trSc[1];
			break;
		case 'o':
			cout<<"\nAbout point: ";
			cin>>dtnew.arrTransfo.param.rot[0]>>dtnew.arrTransfo.param.rot[1];
			cout<<"\nTheta: ";
			cin>>dtnew.arrTransfo.param.rot[2];
			break;
		case 'r':
			cout<<"\nAbout line ax+by+c=0: a= ";
			cin>>dtnew.arrTransfo.param.ref[0];
			cout<<" , b= ";
			cin>>dtnew.arrTransfo.param.ref[1];
			cout<<" , c= ";
			cin>>dtnew.arrTransfo.param.ref[2];
			break;
	}
	if(modi=='b')
	{
		fi.write((char*)&dtnew.getobj(),sizeof(dtnew.getobj()));
		fi.write((char*)&dtnew.arrTransfo,sizeof(dtnew.arrTransfo));
		fi.write((char*)&dti.getobj(),sizeof(dti.getobj()));
		fi.write((char*)&dti.arrTransfo,sizeof(dti.arrTransfo));
	}
	else
	{
		fi.write((char*)&dti.getobj(),sizeof(dti.getobj()));
		fi.write((char*)&dti.arrTransfo,sizeof(dti.arrTransfo));
		fi.write((char*)&dtnew.getobj(),sizeof(dtnew.getobj()));
		fi.write((char*)&dtnew.arrTransfo,sizeof(dtnew.arrTransfo));
	}
}

void fileCopy(ofstream &fi, drawTransform &dti)
{
	fi.write((char*)&dti.getobj(),sizeof(dti.getobj()));
	fi.write((char*)&dti.arrTransfo,sizeof(dti.arrTransfo));
}

void fileModify (ofstream &fi, drawTransform &dti, char modi)
{
	clrscr();
	if(modi=='t')//modify tranfo
	{
		cout<<"\n\n\n                         New Transformation\n\n";
		cout<<"          ****************************************************\n\n";
		cout<<"Type: t:translation, s:scaling, o:rotation, r:reflection :";
		cin>>dti.arrTransfo.type;
		if ((dti.arrTransfo.type !='t')&&(dti.arrTransfo.type !='s')&&(dti.arrTransfo.type !='o')&&(dti.arrTransfo.type !='r'))
		{
			cout<<"\nIncorrect option, assuming translation\n";
			dti.arrTransfo.type='t';
		}
		switch (dti.arrTransfo.type)
		{
			case 't':
				cout<<"\nTranslation x-axis: ";
				cin>>dti.arrTransfo.param.trSc[0];
				cout<<"\nTranslation y-axis: ";
				cin>>dti.arrTransfo.param.trSc[1];
				break;
			case 's':
				cout<<"\nScaling x-axis: ";
				cin>>dti.arrTransfo.param.trSc[0];
				cout<<"\nScaling y-axis: ";
				cin>>dti.arrTransfo.param.trSc[1];
				break;
			case 'o':
				cout<<"\nAbout point: ";
				cin>>dti.arrTransfo.param.rot[0]>>dti.arrTransfo.param.rot[1];
				cout<<"\nTheta: ";
				cin>>dti.arrTransfo.param.rot[2];
				break;
			case 'r':
				cout<<"\nAbout line ax+by+c=0: a= ";
				cin>>dti.arrTransfo.param.ref[0];
				cout<<" , b= ";
				cin>>dti.arrTransfo.param.ref[1];
				cout<<" , c= ";
				cin>>dti.arrTransfo.param.ref[2];
				break;
		}
	}
	else//modify object
	{
		dti.getObjCoord(); //user coordinates
		//convert to screen coord before storing new object
		if(dti.getobj().noVertex==0)
			dti.getobj().xy[0]=dti.findScreenCoord(dti.getobj().xy[0]);
		else
			for(int i=0;i<dti.getobj().noVertex;i++)
				dti.getobj().xy[i]=dti.findScreenCoord(dti.getobj().xy[i]);
	}
	fi.write((char*)&dti.getobj(),sizeof(dti.getobj()));
	fi.write((char*)&dti.arrTransfo,sizeof(dti.arrTransfo));
}


// main function to drive the program
void main()
{
	clrscr();
	int pref, choice, filemenu, cnt=0, cnt1=0,wait, exitProg=0;
	ifstream fin, fin1;
	ofstream fout, fout1, ftmp;
	drawTransform dt[maxVertex];
	char tmpfile[]="tmpfile.dat";
	char GUIfile[]="guifile.txt";
	char Imgfile[20];//="imgfile.dat";//file to be read from
	char Outfile[20];//output file for this run

	//user has 2 choices: read pre-stored obj/transfo/output file or draw/transform from scratch
	do
	{                                                                    
		choice = 1;//draw/transform from scratch
		clrscr();
		cout<<"\n\n\n                         MENU OPTIONS\n\n";
		cout<<"****************************************************************\n\n";
		cout<<"0: Read file of stored object and its transformaions\n";
		cout<<"1: Create a new object and perform transformations\n";
		cout<<"2: Exit the program \n\n";
		cout<<"****************************************************************\n\n";
		cout<<"Please enter your preference (0-2): ";
		cin>> choice;
		if (choice <0 || choice >2) cout<<"\n\nError:Incorrect response, please try again!\n";
	}while (choice <0 || choice>2);
	if (choice==2)	exit(0);
	if (choice == 0)
	{
		cout<<"\nEnter object and transformation input file (binary): ";
		gets(Imgfile);
		//open the file for reading stored image and transformation
		fin1.open(Imgfile,ios::in|ios::binary);
		if (fin1.read((char*)&dt[cnt].getobj(), sizeof(dt[cnt].getobj())))
		{
			//checking that open command was successful
		}
		else{
			cout<<"\n\nError:cannot open img file for reading and writing!\n";
			getch();
			exit (-1);
		}
		fin1.seekg(0,ios::beg);
		//open the tmp file for updating stored image and transformation
		ftmp.open(tmpfile,ios::out|ios::binary|ios::trunc);
		if (!ftmp)
		{
			cout<<"\n\nError:cannot open img file for reading and writing!\n";
			getch();
			exit (-1);
		}
		while (fin1.read((char*)&dt[cnt].getobj(), sizeof(dt[cnt].getobj())))
		{
			fin1.read((char*)&dt[cnt].arrTransfo,sizeof(dt[cnt].arrTransfo));
			//display
			clrscr();
			cout<<"\n\n                              	 Audit Trail \n\n";
			cout<<"***************************************************************************\n\n";
			cout<<"Object#: "<<cnt+1<<",  "<<"# Vertices: "<<dt[cnt].getobj().noVertex<<"\nObject type: ";
			pixel pixDis;
			//cout<<"no of vertex= "<<dt[cnt].getobj().noVertex<<endl;
			//getch();
			switch(dt[cnt].getobj().noVertex)
			{
				case 0://circle/ellipse
					if (dt[cnt].getobj().xy[1].x==dt[cnt].getobj().xy[1].y)//circle
						cout<<"Circle, radius: "<<dt[cnt].getobj().xy[1].x;
					else
						cout<<"Ellipse, radii: "<<dt[cnt].getobj().xy[1].x<<", "<<dt[cnt].getobj().xy[1].y;
					pixDis=dt[cnt].findUserCoord(dt[cnt].getobj().xy[0]);
					cout<<"\nCenter coordinates: ("<<pixDis.x<<", "<<pixDis.y<<")\n";
					cout<<"Start angle(degrees): "<<dt[cnt].getobj().startangle<<", End angle(degrees): "<<dt[cnt].getobj().endangle<<endl;
					break;
				case 1://point
					pixDis=dt[cnt].findUserCoord(dt[cnt].getobj().xy[0]);
					cout<<"Point\ncoordinates: ("<<pixDis.x<<", "<<pixDis.y<<")\n";
					break;
				case 2://line
					pixDis=dt[cnt].findUserCoord(dt[cnt].getobj().xy[0]);
					cout<<"Line\nCoordinates: ("<<pixDis.x<<" , "<<pixDis.y<<") to (";
					pixDis=dt[cnt].findUserCoord(dt[cnt].getobj().xy[1]);
					cout<<pixDis.x<<" , "<<pixDis.y<<")\n";
					break;
				default: //polygon- closed by default
					cout<<"Polygon, closed: "<<dt[cnt].getobj().closedpoly<<"\nCoordinates: ";
					for (int j=0; j<dt[cnt].getobj().noVertex; j++)
					{
						pixDis=dt[cnt].findUserCoord(dt[cnt].getobj().xy[j]);
						cout<<"("<<pixDis.x<<","<<pixDis.y<<")";
						if (j<dt[cnt].getobj().noVertex-1)	cout<<", ";
						else	cout<<endl;
					}
					break;
			}

			cout<<"\nTransformation: ";
			switch (dt[cnt].arrTransfo.type)
			{
				case 't'://translation
					cout<<"Translation by "<<dt[cnt].arrTransfo.param.trSc[0]<<" in x-axis, "<<dt[cnt].arrTransfo.param.trSc[1]<<" in y-axis\n";
					break;
				case 's'://scaling
					cout<<"Scaling by "<<dt[cnt].arrTransfo.param.trSc[0]<<" in x-axis, "<<dt[cnt].arrTransfo.param.trSc[1]<<" in y-axis\n";
					break;
				case 'o'://rotation
					cout<<"Rotation about ("<<(int)dt[cnt].arrTransfo.param.rot[0]<<", "<<(int)dt[cnt].arrTransfo.param.rot[1]<<") by "<<dt[cnt].arrTransfo.param.rot[2]<<" degrees\n";
					break;
				case 'r'://reflection
					cout<<"Reflection about line "<<dt[cnt].arrTransfo.param.ref[0]<<" x + "<<dt[cnt].arrTransfo.param.ref[1]<<" y + "<<dt[cnt].arrTransfo.param.ref[2]<<" = 0\n";
					break;
				default:
					break;
			}
			filemenu = 0;
			cout<<"\n                 		File Operations\n";
			cout<<"***************************************************************************\n";
			cout<<"1: Delete the object and transformation\n";
			cout<<"2: Insert object, transformation before displayed object\n";
			cout<<"3: Insert an object, transformation after displayed object\n";
			cout<<"4: Modify the object\n";
			cout<<"5: Modify the transformation\n";
			cout<<"6: Draw the output object\n";
			cout<<"7: Continue to the next object\n";
			cout<<"Else: Exit\n";
			cout<<"***************************************************************************\n";
			cout<<"Please enter your choice: ";
			cin>>filemenu;
			switch(filemenu)
			{
				case 1://Delete the object and transformation from file
					//Don't write this record in tmp file
					break;
				case 2://Insert new object, transformation before displayed object
					fileInsert(ftmp,dt[cnt],'b');
					break;
				case 3://Insert new object, transformation after displayed object
					fileInsert(ftmp,dt[cnt],'a');
					break;
				case 4://Modify the object
					fileModify(ftmp,dt[cnt],'o');
					break;
				case 5://Modify the transformation
					fileModify(ftmp,dt[cnt],'t');
					break;
				case 6: // work with the object on the screen
					fileCopy(ftmp,dt[cnt]);//copy this record
					break;
				case 7: //continue to next object
					fileCopy(ftmp,dt[cnt]);//copy this record
					break;
				default://do cleanup and exit
					fileCopy(ftmp,dt[cnt]);//copy this record
					exitProg=1;
					break;
			}
			if ((filemenu==6)||(exitProg==1)) break;
			else cnt++;
		}//while
		cnt1=cnt;
		if (filemenu==6 || exitProg==1)//not going till end of file, copy remaining records to tmp file
		{
			// copy rest of the records
			while(1)
			{
				cnt1++;
				if (fin1.read((char*)&dt[cnt1].getobj(), sizeof(dt[cnt1].getobj())))
				{
					fin1.read((char*)&dt[cnt1].arrTransfo,sizeof(dt[cnt1].arrTransfo));
					ftmp.write((char*)&dt[cnt1].getobj(),sizeof(dt[cnt1].getobj()));
					ftmp.write((char*)&dt[cnt1].arrTransfo,sizeof(dt[cnt1].arrTransfo));
				}
				else break;
			}
		}
		if (fin1)	fin1.close();
		if (ftmp)	ftmp.close();
		remove(Imgfile);
		rename(tmpfile,Imgfile);
	}//if (choice==0)
	if (choice==1 || filemenu==6)//input object , transfo from scratch, store when (choice=1)
	{
		if (filemenu!=6)	dt[cnt].setObjDefault(); //setting the object default values
		//initialize graphics for taking screen coordinates
		int gd=DETECT, gm;
		char kb;
		initgraph(&gd, &gm,"c:\\turboc3\\bgi");

		dt[cnt].getobj().maxx=getmaxx();//max x coordinates of my screen=639
		dt[cnt].getobj().maxy=getmaxy();//max y coordinates of my screen=439
		dt[cnt].getobj().usermaxx=dt[cnt].getobj().maxx/2;
		dt[cnt].getobj().usermaxy=dt[cnt].getobj().maxy/2;
		closegraph();
		//max screen coordinates: dt.getobj().maxx=639, dt.getobj().maxy=479
		//max coordinates on user coordinate system
		//x-axis: -315 to 315; y-axis: -235 to 235

		//take parameters for the object to be drawn
		if (filemenu!=6)	dt[cnt].getObjCoord ();

		//get the user preference (pref) for where to take GUI parameters from
		//pref==0 => take default GUI options
		//pref==1 => take gui options from a stored file, if file not present, choice of default, from screen
		//pref=2 => take gui options from screen
		do
		{
			pref = 0;
			clrscr();
			cout<<"\n\n\n                         GUI Input Preference\n\n";
			cout<<"**************************************************************\n\n";
			cout<<"0: Default GUI options\n";
			cout<<"1: GUI options from user defined file \"guifile.txt\"\n";
			cout<<"2: Enter the GUI options from screen menus\n\n";
			cout<<"**************************************************************\n\n";
			cout<<"Please enter your preference (0-2): ";
			cin>>	pref;
			if (pref <0 || pref >2) cout<<"\nIncorrect response, please try again\n";
		}while (pref <0 || pref >2);
		if (pref==1)
		{
			fin.open(GUIfile,ios::in);
			if (!fin)
			{
				cout<<"\n\nError:cannot open guifile.txt!\n";
				getch();
				do
				{
					pref = 0;
					clrscr();
					cout<<"\n\n\n                         GUI Input Preference\n\n";
					cout<<"**************************************************************\n\n";
					cout<<"0: Default GUI options\n";
					cout<<"2: Enter the GUI options from screen menus\n\n";
					cout<<"**************************************************************\n\n";
					cout<<"Please enter your preference (0/2): ";
					cin>>pref;
					if (pref !=0 && pref !=2) cout<<"\nIncorrect response, please try again\n";
					//getch();
				}while (pref !=0 && pref !=2);
			}
		}
		cout<<"\nEnter object and transformation output file (binary): ";
		gets(Outfile);
		//reinitialize graphics
		gd=DETECT, gm=DETECT;
		initgraph(&gd, &gm,"c:\\turboc3\\bgi");

		//get graphics parameters
		dt[cnt].getGUI(pref,fin);
		dt[cnt].drawCoord();//draw coordinate system in default color-white

		//converting user to screen coordinates if taken a fresh object
		//objects are stored in file in screen coordinates
		if (filemenu==6)
			wait=0;//during transfo fn draw without waiting for input
		else
		{
			wait=1;
			if(dt[cnt].getobj().noVertex==0)
				dt[cnt].getobj().xy[0]=dt[cnt].findScreenCoord(dt[cnt].getobj().xy[0]);
			else
				for(int i=0;i<dt[cnt].getobj().noVertex;i++)
					dt[cnt].getobj().xy[i]=dt[cnt].findScreenCoord(dt[cnt].getobj().xy[i]);
		}
		fout1.open(Outfile,ios::out|ios::trunc|ios::binary);
		if (!fout1)
		{
			cout<<"\n\nError:cannot open output file for saving objects, transfo!\n";
			getch();
			exit(-1);
		}
		dt[cnt].drawObj();
		drawTransform dttmp;
		do{
			dttmp.getobj()=dt[cnt].getobj();
			kb=dt[cnt].transfo(wait);
			if ((kb=='1' || kb=='2' || kb=='3' || kb=='4') && (wait==1) && fout1)//wait==0 => rendering- don't store
			{
				//store the object and transformation
				fout1.write((char*)&dttmp.getobj(),sizeof(dttmp.getobj()));
				fout1.write((char*)&dt[cnt].arrTransfo,sizeof(dt[cnt].arrTransfo));
			}

			wait=1;//now user can enter more transformations directly
		}while(kb!='5');
		if (fin)
			fin.close();
		if (fout1)
			fout1.close();
		closegraph();
		//if new GUI options were input from screen, save
		if (pref==2)
		{
			//saving the final value of GUI parameters in the file
			fout.open(GUIfile,ios::out|ios::trunc);
			if (!fout)
				cout<<"\n\nError:cannot open guifile.txt file for saving GUI parameters!\n";
			else
			{
				fout<<dt[cnt].getobj().closedpoly<<endl;
				fout<<dt[cnt].getgui().color<<endl;
				fout<<dt[cnt].getgui().coordcolor<<endl;
				fout<<dt[cnt].getgui().bkcolor<<endl;
				fout<<dt[cnt].getgui().linestyle<<endl;
				fout<<dt[cnt].getgui().linewidth<<endl;
				fout<<dt[cnt].getgui().fillstyle<<endl;
				fout<<dt[cnt].getgui().fillcolor<<endl;
				fout<<dt[cnt].getgui().textfont<<endl;
				fout<<dt[cnt].getgui().textdir<<endl;
				fout<<dt[cnt].getgui().usrpattern<<endl;
				fout<<dt[cnt].getgui().charsize<<endl;
				fout.close();
			}
		}
	}//(choice==1 || filemenu==6)
	cout<<"Goodbye!! Please press any key to exit....";
	getch();
}

