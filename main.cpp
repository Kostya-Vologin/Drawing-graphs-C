#include "tinyexpr.h"
#include "graphics.h"
#include <stdio.h>
#include <string.h>
#define MAX_EQUATIONS 10
#define MAX_EQUATION_LENGTH 20

int wx=800, wy=600; // wy = (wx*2)/3
int Lx=wx/3+wy/20, Ly=wy/20; //координата левой верхней точки поля для рисования графиков
int Nx=wx-wy/20, Ny=wy-(wy/10+wy/20); //координата правой нижней точки поля для рисования графиков
int x0, y0; //координата пересечения осей

typedef struct {
   char equations[MAX_EQUATIONS][MAX_EQUATION_LENGTH]; // Массив для хранения уравнений
   int error_index[MAX_EQUATIONS]; // Индекс массива, в котором допущена ошибка (-1 если ошибок нет)
} Equation;
Equation eqSet;
int text_x=20, text_y=0; //координаты уравнений
int f_color[MAX_EQUATIONS];

const char *X1="Выбор диапазона по X: min - ";
const char *X2=", max - ";
int ax=wx/3+wy/20; // двигаем текст по X
int bx=wy-60; // двигаем текст по Y
const char *Y1="Выбор диапазона по Y: min - ";
const char *Y2=", max - ";
int cy=wy-35; // двигаем текст по Y

int recx[5];//Точки X начального четырёх rectangle (диапазоны)
double x_min=-30, x_max=30; //целые числа, определяющие начальное и конечное значение диапазона по оси X
double y_min=-30, y_max=30; //целые числа, определяющие начальное и конечное значение диапазона по оси Y
char buffer1[10]="-30", buffer2[10]="30", buffer3[10]="-30", buffer4[10]="30";

void background() {
   /*---------------- Рисуем окно: ----------------*/
   setfillstyle(SOLID_FILL, WHITE);
   bar(0, 0, wx, wy);

   setcolor(BLACK);
   line(0, 0, 0, wy);
   int t=wy/10;
   int a=0;
   while (a<=wy) {
      line(0, a, wx/3, a);
      a+=t;
   }
   rectangle(wx/3, 0, wx, wy);
   rectangle(Lx-1, Ly-1, Nx, Ny);

   /*----------------- Диапазоны: -----------------*/
   setbkcolor(WHITE);

   outtextxy(ax, bx, X1);
   rectangle(recx[0], bx, recx[0]+40, bx+20);
   outtextxy(recx[0]+42, bx, X2);
   rectangle(recx[1], bx, recx[1]+40, bx+20);

   outtextxy(ax, cy, Y1);
   rectangle(recx[2], cy, recx[2]+40, cy+20);
   outtextxy(recx[2]+42, cy, Y2);
   rectangle(recx[3], cy, recx[3]+40, cy+20);
}

void draw_cursor(int x, int y, int c) { // рисование мигающего курсора
   setcolor(c);
   line(x, y, x, y+textheight("A")-1);
}

void clear_character(int x, int y) { // удаление символа
   setfillstyle(SOLID_FILL, WHITE);
   bar(x, y, x+textwidth("A")+5, y+textheight("A"));
}

void entering(int x, int y, char *equations, int MAX_LENGTH, const char *dop, int type, int nomer) { // ввод строки
   int len=strlen(equations);
   setcolor(BLACK);
   setbkcolor(WHITE);
   int cur=0;
   int ch;
   while (!ismouseclick(WM_LBUTTONDOWN)) {
      draw_cursor(x+textwidth(equations)+2, y, cur<10?BLACK:WHITE);
      cur=(cur+1)%20;
      if (!kbhit()) delay(50);
      else {
         ch=getch(1);
         if (ch==KEY_ENTER || ch==KEY_ESC) break;
         else if (ch==KEY_SPECIAL+KEY_SHIFT) continue;
         else if (ch==KEY_BACKSPACE && len>0) {
            len--;
            equations[len]='\0'; // удаление последнего символа
            clear_character(x+textwidth(equations), y);// удаление с экрана
            //setcolor(BLACK);
            //outtextxy(x, y, equations);
         } else if (strchr(dop, ch)!=NULL && len<MAX_LENGTH-1) {
            equations[len]=ch;
            len++;
            equations[len]='\0';
            setcolor(BLACK);
            outtextxy(x, y, equations);
         }
      }
   }
   draw_cursor(x+textwidth(equations)+2, y, WHITE);
   if (type==2) {
      double *a;
      switch (nomer) {
      case 0:
         a=&x_min;
         break;
      case 1:
         a=&x_max;
         break;
      case 2:
         a=&y_min;
         break;
      case 3:
         a=&y_max;
         break;
      }
      sscanf(equations, "%lf", a);
   }
}

void entering_equations(int x, int y) { // ввод уравнений
   int equation_area=y/(wy/10);
   text_y=(equation_area*wy/10)+24;
   entering(text_x, text_y, eqSet.equations[equation_area], MAX_EQUATION_LENGTH, "0123456789abcdefghijklmnopqrstuvwxyz()+-*/^=!.", 1, -1);
}

typedef struct { // структура для диапазанов
   int start_x;
   int end_x;
   int start_y;
   int end_y;
   char *buffer;
   int r;
   int u;
} Range;

bool entering_ranges(int x, int y) { // ввод диапазонов
   Range ranges[]= {
      {recx[0], recx[0]+40, bx, bx+20, buffer1, recx[0]+2, bx+2},
      {recx[1], recx[1]+40, bx, bx+20, buffer2, recx[1]+2, bx+2},
      {recx[2], recx[2]+40, cy, cy+20, buffer3, recx[2]+2, cy+2},
      {recx[3], recx[3]+40, cy, cy+20, buffer4, recx[3]+2, cy+2}
   };

   for (int i=0; i<4; i++) {
      if (x>ranges[i].start_x && x<ranges[i].end_x && y>ranges[i].start_y && y<ranges[i].end_y) {
         entering(ranges[i].r, ranges[i].u, ranges[i].buffer, 5, "+-0123456789.", 2, i);
         return true;
      }
   }
   return false;
}

void drawing_graph(int equation_index) { // функция для рисования графиков
   char a[MAX_EQUATION_LENGTH];
   if (eqSet.equations[equation_index][0]!='y' || eqSet.equations[equation_index][1]!='=') { // проверка, что уравнение начинается "y="
      eqSet.error_index[equation_index]=1;
      return;
   }
   strcpy(a, eqSet.equations[equation_index]+2); // удаляем y=
   const char *equation=a; // yравнение

   int bufx, bufy;
   double x, y;
   int err;
   te_variable vars[] = {{"x", &x}};
   te_expr *expr = te_compile(equation, vars, 1, &err);

   if (expr) {
      double step=(x_max-x_min)/1000.0;

      x=x_min;
      y=te_eval(expr);
      bufx=(x-x_min)*(Nx-Lx)/(x_max-x_min);
      bufy=(y-y_max)*(Ny-Ly)/(y_min-y_max);

      setcolor(f_color[equation_index]);
      setlinestyle(SOLID_LINE, 0, 2);
      for (x=x_min+step; x<=x_max; x+=step) {
         y=te_eval(expr);
         int xp=(x-x_min)*(Nx-Lx)/(x_max-x_min);
         int yp=(y-y_max)*(Ny-Ly)/(y_min-y_max);
         line(bufx, bufy, xp, yp);
         bufx=xp;
         bufy=yp;
      }
      te_free(expr);
      eqSet.error_index[equation_index]=-1;
   } else {
      eqSet.error_index[equation_index]=1;
   }
   setlinestyle(SOLID_LINE, 0, 1);
}

double selectstep(double dmin, double dmax) { // функция для вычисления stepx, stepy
   double step=1;
   for (int i=0; (dmax-dmin)/step>20; i++) {
      if (i%2==0) step*=2.0;
      else step*=5.0;
   }
   for (int i=0; (dmax-dmin)/step<5; i++) {
      if (i%2==0) step/=2.0;
      else step/=5.0;
   }
   return step;
}

void draw(double i, double v, const char *axis) { // функция для вывода сетки, а также координат
   char buffer[10];
   sprintf(buffer, "%g", v);
   setcolor(LIGHTGRAY);
   if (axis=="x") line(i, 0, i, Ny-Ly);
   else line(0, i, Nx-Lx, i);
   setcolor(BLACK);
   setbkcolor(NO_COLOR);
   if (axis=="x") outtextxy(i-textwidth(buffer)/2, y0+1, buffer);
   else outtextxy(x0+2, i-textheight(buffer)/2, buffer);
}

void redrawing_area() { // функция для перерисовки области с графиком
   setviewport(Lx, Ly, Nx, Ny, 1);
   clearviewport();
   double stepx, stepy; //размер стороны клеточки
   double mx=(Nx-Lx)/(x_max-x_min);
   double my=(Ny-Ly)/(y_max-y_min);
   stepx=selectstep(x_min, x_max);
   stepy=selectstep(y_min, y_max);

   x0=(0-x_min)*mx;
   y0=(0-y_max)*(-my);

   double v=stepx;
   for (double i=x0+stepx*mx; i<Nx-Lx; i+=stepx*mx, v+=stepx) draw(i, v, "x");
   v=-stepx;
   for (double i=x0-stepx*mx; i>0; i-=stepx*mx, v-=stepx) draw(i, v, "x");
   double u=stepy;
   for (double i=y0-stepy*my; i>0; i-=stepy*my, u+=stepy) draw(i, u, "y");
   u=-stepy;
   for (double i=y0+stepy*my; i<Ny-Ly; i+=stepy*my, u-=stepy) draw(i, u, "y");
   setcolor(BLACK);
   setbkcolor(NO_COLOR);
   outtextxy(x0+2, y0+1, "0");

   //рисование осей:
   setlinestyle(SOLID_LINE, 0, 2);
   line(0, y0, Nx-Lx, y0);
   line(x0, 0, x0, Ny-Ly);

   line(Nx-Lx, y0, Nx-Lx-5, y0-5);
   line(Nx-Lx, y0, Nx-Lx-5, y0+5);
   line(x0, 0, x0+5, 5);
   line(x0, 0, x0-5, 5);
   setlinestyle(SOLID_LINE, 0, 1);

   // цикл перерисовки графиков:
   for (int i=0; i<MAX_EQUATIONS; i++) {
      if (eqSet.equations[i][0]!='\0') { // Проверка на пустой массив
         drawing_graph(i);
      }
   }
   setviewport(0, 0, getmaxx(), getmaxy(), 1);
}

int main() {
   initwindow(wx, wy, "Program");
   for (int i=0; i<MAX_EQUATIONS; i++) {
      eqSet.error_index[i]=-1; // Изначально ошибок в уравнениях нет
   }
   f_color[0]=COLOR(200, 0, 0); // Красный
   f_color[1]=COLOR(0, 200, 0); // Зеленый
   f_color[2]=COLOR(0, 0, 200); // Синий
   f_color[3]=COLOR(200, 100, 0); // Оранжевый
   f_color[4]=COLOR(128, 0, 255); // Фиолетовый
   f_color[5]=COLOR(0, 204, 204); // Темно-бирюзовый
   f_color[6]=COLOR(128, 0, 0); // Бордовый
   f_color[7]=COLOR(0, 128, 255); // Голубой
   f_color[8]=COLOR(255, 0, 128); // Розовый
   f_color[9]=COLOR(128, 128, 0); // Оливковый

   recx[0]=ax+textwidth(X1)+2;
   recx[1]=ax+textwidth(X1)+44+textwidth(X2)+2;
   recx[2]=ax+textwidth(Y1)+2;
   recx[3]=ax+textwidth(Y1)+44+textwidth(Y2)+2;
   background();
   redrawing_area();
   outtextxy(recx[0]+2, bx+2, buffer1);
   outtextxy(recx[1]+2, bx+2, buffer2);
   outtextxy(recx[2]+2, cy+2, buffer3);
   outtextxy(recx[3]+2, cy+2, buffer4);

   int x, y;
   while (1) {
      if (ismouseclick(WM_LBUTTONDOWN)) {
         clearmouseclick(WM_LBUTTONDOWN);
         x=mousex();
         y=mousey();
         if (x<wx/3) {
            entering_equations(x, y);
            redrawing_area();
            int o=wx/3-20;
            for (int i=0; i<10; i++) {
               int c=wy*(0.1*i+0.05);
               if (eqSet.equations[i][0]!='\0' && eqSet.error_index[i]!=1) {
                  setcolor(f_color[i]);
                  setfillstyle(SOLID_FILL, f_color[i]);
                  fillellipse(o, c, 5, 5);
               } else {
                  setcolor(WHITE);
                  setfillstyle(SOLID_FILL, WHITE);
                  fillellipse(o, c, 5, 5);
               }
            }
            for (int i=0; i<MAX_EQUATIONS; i++) {
               if (eqSet.error_index[i]==1) {
                  setcolor(RED);
                  outtextxy(text_x, (i*wy/10)+24, eqSet.equations[i]); // Вывод всех строк уравнений
                  setcolor(BLACK);
               }
            }
         }
         if (entering_ranges(x, y)) {
            if (x_min>x_max || y_min>y_max || x_min==x_max || y_min==y_max) {
               setcolor(RED);
               setbkcolor(WHITE);
               outtextxy(Lx, Ny+4, "Пожалуйста, исправьте диапазоны. Должно быть: min<max");
               continue;
            } else {
               setfillstyle(SOLID_FILL, WHITE);
               bar(Lx, Ny+4, Lx+402, Ny+20);
            }
            redrawing_area();
         }
      }
   }
   closegraph();
   return 0;
}