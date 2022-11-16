// Cursach.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <stdio.h>
#include <GL/freeglut.h>//собственно нужный нам заголовочный файл
#include <math.h>
#include <dos.h>

int LENGTH = 15;
int WIDTH = 15;
int size_square = 50;
int** field;
float ATTACK_WEIGHT[6][3];



// 1 - крестик, 2 - нолик

int dX = 0;
int dY = 0;
int leftPress = 0;

int X = 0;
int Y = 0;
typedef enum Tstep {
    CROSS =1,
    CIRCLE =2,
}STEP;

STEP Playerstep = 0;
STEP Botstep = 0;

STEP win = 0;

typedef struct attack {
    int x;
    int y;
    float rez;
}attack;

void drawField() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 0.0f);

    for (int i = 0; i <= LENGTH; i++) {
        glVertex2f(i* size_square, 0);//координата начала отрезка в формате (x,y)
        glVertex2f(i* size_square, WIDTH *size_square);//координата конца отрезка
    }
    for (int i = 0; i <= WIDTH; i++) {
        glVertex2f(0,i * size_square);//координата начала отрезка в формате (x,y)
        glVertex2f(LENGTH*size_square, i * size_square);//координата конца отрезка
    }
    //glEnd();
}

void drawSigns() {
    for (int i = 0; i < LENGTH; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (field[i][j] == 1) {
                glColor3f(1.0f, 0.0f, 0.0f);
                float pointX = i * size_square;
                float pointY = j * size_square;
                glVertex2f(pointX, pointY); glVertex2f(pointX + size_square, pointY + size_square);
                glVertex2f(pointX, pointY + size_square); glVertex2f(pointX + size_square, pointY);
            }
            if (field[i][j] == 2) {
                glColor3f(0.0f, 0.0f, 1.0f);
                float centerX = size_square * (i + 0.5);
                float centerY = size_square * (j + 0.5);
                for (int k = 0; k < 360; k++) {
                    glVertex2f(centerX, centerY); glVertex2f(centerX + cos((double)k * 3.14 / 180) * (size_square / 2), centerY + sin((double)k * 3.14 / 180) * (size_square / 2));
                }
            }
        }
    }
}

void control(int x, int y, STEP step) {
    int k = 0;

    for (int i = 1; i < 5; i++) {
        if ((x - i < 0) || (field[x - i][y] != step))break;
        k++;
    }
    for (int i = 1; i < 5; i++) {
        if ((x + i >= LENGTH) || (field[x + i][y] != step))break;
        k++;
    }
    if (k >= 4) {
        win = step;
    }
    k = 0;

    for (int i = 1; i < 5; i++) {
        if ((y - i < 0)||(field[x][y - i] != step))break;
        k++;
    }
    for (int i = 1; i < 5; i++) {
        if ((y + i >= WIDTH)||(field[x][y + i] != step))break;
        k++;
    }
    if (k >= 4) {
        win = step;
    }
    k = 0;

    for (int i = 1; i < 5; i++) {
        if ((x - i < 0) || (y - i < 0) || (field[x - i][y - i] != step))break;
        k++;
    }
    for (int i = 1; i < 5; i++) {
        if ((x + i >= LENGTH) || (y + i >= WIDTH)||(field[x + i][y + i] != step))break;
        k++;
    }
    if (k >= 4) {
        win = step;
    }
    k = 0;

    for (int i = 1; i < 5; i++) {
        if ((x - i < 0) || (y + i >= WIDTH)||(field[x - i][y + i] != step))break;
        k++;
    }
    for (int i = 1; i < 5; i++) {
        if ((x + i >= LENGTH) || (y - i < 0)||(field[x + i][y - i] != step))break;
        k++;
    }
    if (k >= 4) {
        win = step;
    }
    k = 0;
}



void botStep(STEP step) {
    attack playerMax = { 0,0,0.0 };
    attack botMax = { 0,0,0.0 };
    int mosh = 1;
    int potenc = 0;
    float tmpAttack = 0;


    for (int x = 0; x < LENGTH; x++) {
        for (int y = 0; y < WIDTH; y++) {
            if (field[x][y] != 0)continue;
            tmpAttack = 0;

            //gorizontal
            for (int i = 1; i < 5; i++) {
                if ((x - i < 0)) break;
                if (field[x - i][y] == 0) {
                    potenc++;
                    break;
                };
                if (field[x - i][y] != step) break;
                if (field[x - i][y] == step) mosh++;
            }
            for (int i = 1; i < 5; i++) {
                if ((x + i >= LENGTH)) break;
                if (field[x + i][y] == 0) {
                    potenc++;
                    break;
                };
                if (field[x + i][y] != step) break;
                if (field[x + i][y] == step) mosh++;
            }
            tmpAttack += ATTACK_WEIGHT[mosh][potenc];
            mosh = 1;
            potenc = 0;

            // Vertical
            for (int i = 1; i < 5; i++) {
                if ((y - i < 0)) break;
                if (field[x][y - i] == 0) {
                    potenc++;
                    break;
                };
                if (field[x][y - i] != step) break;
                if (field[x][y - i] == step) mosh++;
            }
            for (int i = 1; i < 5; i++) {
                if ((y + i >= WIDTH)) break;
                if (field[x][y + i] == 0) {
                    potenc++;
                    break;
                };
                if (field[x][y + i] != step) break;
                if (field[x][y + i] == step) mosh++;
            }
            tmpAttack += ATTACK_WEIGHT[mosh][potenc];
            mosh = 1;
            potenc = 0;


            // diagonal
            for (int i = 1; i < 5; i++) {
                if ((y - i < 0) || (x - i < 0)) break;
                if (field[x - i][y - i] == 0) {
                    potenc++;
                    break;
                };
                if (field[x - i][y - i] != step) break;
                if (field[x - i][y - i] == step) mosh++;
            }
            for (int i = 1; i < 5; i++) {
                if ((y + i >= WIDTH) || (x + i >= LENGTH)) break;
                if (field[x + i][y + i] == 0) {
                    potenc++;
                    break;
                };
                if (field[x + i][y + i] != step) break;
                if (field[x + i][y + i] == step) mosh++;
            }
            tmpAttack += ATTACK_WEIGHT[mosh][potenc];
            mosh = 1;
            potenc = 0;

            for (int i = 1; i < 5; i++) {
                if ((y - i < 0) || (x + i >= LENGTH)) break;
                if (field[x + i][y - i] == 0) {
                    potenc++;
                    break;
                };
                if (field[x + i][y - i] != step) break;
                if (field[x + i][y - i] == step) mosh++;
            }
            for (int i = 1; i < 5; i++) {
                if ((y + i >= WIDTH) || (x - i < 0)) break;
                if (field[x - i][y + i] == 0) {
                    potenc++;
                    break;
                };
                if (field[x - i][y + i] != step) break;
                if (field[x - i][y + i] == step) mosh++;
            }
            tmpAttack += ATTACK_WEIGHT[mosh][potenc];
            mosh = 1;
            potenc = 0;

            if (tmpAttack >= botMax.rez) {
                botMax.x = x;
                botMax.y = y;
                botMax.rez = tmpAttack;
            }

            tmpAttack = 0;

            for (int i = 1; i < 5; i++) {
                if ((x - i < 0)) break;
                if (field[x - i][y] == 0) {
                    potenc++;
                    break;
                };
                if (field[x - i][y] != Playerstep) break;
                if (field[x - i][y] == Playerstep) mosh++;
            }
            for (int i = 1; i < 5; i++) {
                if ((x + i >= LENGTH)) break;
                if (field[x + i][y] == 0) {
                    potenc++;
                    break;
                };
                if (field[x + i][y] != Playerstep) break;
                if (field[x + i][y] == Playerstep) mosh++;
            }
            tmpAttack += ATTACK_WEIGHT[mosh][potenc];
            mosh = 1;
            potenc = 0;


            for (int i = 1; i < 5; i++) {
                if ((y - i < 0)) break;
                if (field[x][y - i] == 0) {
                    potenc++;
                    break;
                };
                if (field[x][y - i] != Playerstep) break;
                if (field[x][y - i] == Playerstep) mosh++;
            }
            for (int i = 1; i < 5; i++) {
                if ((y + i >= WIDTH)) break;
                if (field[x][y + i] == 0) {
                    potenc++;
                    break;
                };
                if (field[x][y + i] != Playerstep) break;
                if (field[x][y + i] == Playerstep) mosh++;
            }
            tmpAttack += ATTACK_WEIGHT[mosh][potenc];
            mosh = 1;
            potenc = 0;



            for (int i = 1; i < 5; i++) {
                if ((y - i < 0) || (x - i < 0)) break;
                if (field[x - i][y - i] == 0) {
                    potenc++;
                    break;
                };
                if (field[x - i][y - i] != Playerstep) break;
                if (field[x - i][y - i] == Playerstep) mosh++;
            }
            for (int i = 1; i < 5; i++) {
                if ((y + i >= WIDTH) || (x + i >= LENGTH)) break;
                if (field[x + i][y + i] == 0) {
                    potenc++;
                    break;
                };
                if (field[x + i][y + i] != Playerstep) break;
                if (field[x + i][y + i] == Playerstep) mosh++;
            }
            tmpAttack += ATTACK_WEIGHT[mosh][potenc];
            mosh = 1;
            potenc = 0;

            for (int i = 1; i < 5; i++) {
                if ((y - i < 0) || (x + i >= LENGTH)) break;
                if (field[x + i][y - i] == 0) {
                    potenc++;
                    break;
                };
                if (field[x + i][y - i] != Playerstep) break;
                if (field[x + i][y - i] == Playerstep) mosh++;
            }
            for (int i = 1; i < 5; i++) {
                if ((y + i >= WIDTH) || (x - i < 0)) break;
                if (field[x - i][y + i] == 0) {
                    potenc++;
                    break;
                };
                if (field[x - i][y + i] != Playerstep) break;
                if (field[x - i][y + i] == Playerstep) mosh++;
            }
            tmpAttack += ATTACK_WEIGHT[mosh][potenc];
            mosh = 1;
            potenc = 0;

            if (tmpAttack >= playerMax.rez) {
                playerMax.x = x;
                playerMax.y = y;
                playerMax.rez = tmpAttack;
            }
        }
    }

    if (playerMax.rez >= botMax.rez)field[playerMax.x][playerMax.y] = Botstep;
    else {
        field[botMax.x][botMax.y] = Botstep;
        control(botMax.x, botMax.y, Botstep);
    }
}

void mousePressed(int button, int state, int ax, int ay) {
    if (Playerstep == 0) {
        if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
            if (ax < 500) {
                Playerstep = CROSS;
                Botstep = CIRCLE;
            }
            else {
                Playerstep = CIRCLE;
                Botstep = CROSS;
                botStep(Botstep);
            }

        }
    }
    else {
        if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
            dX = ax;
            dY = ay;
        }
        if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP)) {
            dX = ax - dX;
            dY = ay - dY;
        }
        if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
            if ((((ax - X) / size_square) > LENGTH) || (((ax - X) / size_square) < 0) ||
                (((ay - Y) / size_square) > WIDTH) || (((ay - Y) / size_square) < 0))
                return;
            if (field[(ax - X) / size_square][(ay - Y) / size_square] == 0) {
                field[(ax - X) / size_square][(ay - Y) / size_square] = Playerstep;
                control((ax - X) / size_square, (ay - Y) / size_square, Playerstep);
                

                if (LENGTH - ((ax - X) / size_square) < 3) {

                    field = (int**)realloc(field, sizeof(int*) * (LENGTH + 5));
                    
                    field[LENGTH] = (int*)calloc(WIDTH, sizeof(int));
                    field[LENGTH + 1] = (int*)calloc(WIDTH, sizeof(int));
                    field[LENGTH + 2] = (int*)calloc(WIDTH, sizeof(int));
                    field[LENGTH + 3] = (int*)calloc(WIDTH, sizeof(int));
                    field[LENGTH + 4] = (int*)calloc(WIDTH, sizeof(int));
                    
                    LENGTH += 5;
                }
                if (WIDTH - ((ay - Y) / size_square) < 3) {
                    for (int i = 0; i < LENGTH; i++) {
                        field[i] = (int*)realloc(field[i], sizeof(int) * (WIDTH+5));
                        field[i][WIDTH] = 0;
                        field[i][WIDTH + 1] = 0;
                        field[i][WIDTH + 2] = 0;
                        field[i][WIDTH + 3] = 0;
                        field[i][WIDTH + 4] = 0;
                    }
                    WIDTH += 5;
                }


                botStep(Botstep);
            }
        }
    }
}

void display()
{
    if(Playerstep==0){
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(500, 0); glVertex2f(500, 1000);


    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0, 250); glVertex2f(500, 750);
    glVertex2f(0, 750); glVertex2f(500, 250);


    glColor3f(0.0f, 0.0f, 1.0f);
    for (int k = 0; k < 1440; k++) {
        glVertex2f(750, 500); glVertex2f(750 + cos((double)k * 3.14 / 720) * 250, 500 + sin((double)k * 3.14 / 720) * 250);
    }

    glEnd();
    glutSwapBuffers();//прорисовываем буфер на экран/* рисуем что нибудь */
    }
    else {

        if (win == 0) {
            glTranslatef(dX, dY, 0);
            glBegin(GL_LINES);
            drawField();
            drawSigns();
            glEnd();

            glutSwapBuffers();//прорисовываем буфер на экран/* рисуем что нибудь */
            glClear(GL_COLOR_BUFFER_BIT);
            X += dX;
            Y += dY;
            dX = 0;
            dY = 0;
        }
        else {
            glTranslatef(-X, -Y, 0);
            glBegin(GL_LINES);
            if (win == CROSS) {

                glColor3f(1.0f, 0.0f, 0.0f);
                glVertex2f(250, 0); glVertex2f(750, 500);
                glVertex2f(250, 500); glVertex2f(750, 0);
            }
            else {
                glColor3f(0.0f, 0.0f, 1.0f);
                for (int k = 0; k < 1440; k++) {
                    glVertex2f(500, 250); glVertex2f(500 + cos((double)k * 3.14 / 720) * 250, 250 + sin((double)k * 3.14 / 720) * 250);
                }
            }
            glEnd();
            glutSwapBuffers();
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }
}







void mouse(int x, int y) {
    printf("%d  %d\n", x, y);
}
int main(int argc, char** argv)
{
    ATTACK_WEIGHT[1][1] = 0.1;
    ATTACK_WEIGHT[2][1] = 2;
    ATTACK_WEIGHT[3][1] = 4;
    ATTACK_WEIGHT[4][1] = 6;
    ATTACK_WEIGHT[5][1] = 200;

    ATTACK_WEIGHT[1][2] = 0.25;
    ATTACK_WEIGHT[2][2] = 5;
    ATTACK_WEIGHT[3][2] = 7;
    ATTACK_WEIGHT[4][2] = 100;
    ATTACK_WEIGHT[5][2] = 200;

    ATTACK_WEIGHT[5][0] = 200;

    field = (int**)malloc(LENGTH * sizeof(int*));
    for (int i = 0; i < LENGTH; i++) {
        field[i] = (int*)calloc(WIDTH, sizeof(int));
    }
    
    glutInit(&argc, argv);//начальная инициализация окна
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ACCUM);//установка режима отображения
    glutInitWindowSize(1000, 1000);//размер окна
    glutInitWindowPosition(0, 0);//начальная позиция на экране
    glutCreateWindow("Test");//заголовок окна
    glClearColor(1, 1, 1, 0);//цвет фона

    // настройка проекции, с этими двумя ф-ми познакомимся поближе чуть позже.
    glMatrixMode(GL_PROJECTION);//режим матрицы
    glLoadIdentity();//отчищает матрицу
    //glutPassiveMotionFunc(mouse);
    glutMouseFunc(mousePressed);
    gluOrtho2D(0, 1000, 1000, 0);//cоздаем пространство нашей сцены, в данном случае 3D пространство с высотой, шириной
    glutDisplayFunc(display);//функция которой мы передаем имя функции для отрисовки окна. 
    glutMainLoop();//запускаем всё проинициализированное, проще говоря та же ф-я main, только в данном случае glut'овская ф-я main.

    return 0;
}
