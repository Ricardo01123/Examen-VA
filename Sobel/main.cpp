///////////////////////////////Cabeceras/////////////////////kk///////
#define _USE_MATH_DEFINES
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>
///////////////////////////////Espacio de nombres////////////////////////////
using namespace cv;
using namespace std;

////////////////////////////////Lectura de la Imagen/////////////////////////
Mat obtenerImagen(char NombreImagen[]) {

    Mat imagen; // Matriz que contiene nuestra imagen sin importar el formato


    /*********Lectura de la imagen*********/
    imagen = imread(NombreImagen);

    if (!imagen.data)
    {
        cout << "Error al cargar la imagen: " << NombreImagen << endl;
        exit(1);
    }
    return imagen;
}

////////////////////////////////---**Gauss**---/////////////////////////////////
////////////////////////////////---**Generar Kernel**--/////////////////////////
//g(x,y) = T[f(x,y)]
vector<vector<float>> mascaraKernerl(int Kernelsize, float sigma) {

    int limite = (Kernelsize - 1) / 2; //ubica el centro
    float r, s, z, gaussResultado;
    float sum = 0.0;

    vector<vector<float>> mascara(Kernelsize, vector<float>(Kernelsize, 0));

    s = 2.0 * sigma * sigma;

    for (int x = -limite; x <= limite; x++) {
        for (int y = -limite; y <= limite; y++) {
            r = sqrt(x * x + y * y);
            z = (exp(-(r * r) / s)) / (M_PI * s);
            gaussResultado = (exp(-(r * r) / s)) / (M_PI * s);
            mascara[x + limite][y + limite] = gaussResultado;
            cout << gaussResultado << "\t";
            sum += gaussResultado;

        }
        cout << "\n";
    }

    for (int i = 0; i < Kernelsize; i++) {
        for (int j = 0; j < Kernelsize; j++) {
            mascara[i][j] /= sum;
            //cout << mascara[i][j] << endl;
        }
    }

    return mascara;
}

////////////////////////////////---**Sobel**--////////////////////////////////////
//******************************------Gx------///////////////////////////////////
vector<vector<float>> gy() {

    vector<vector<float>> mascara(3, vector<float>(3, 0));

    mascara[0][0] = -1;
    mascara[0][1] = -2;
    mascara[0][2] = -1;

    mascara[1][0] = 0;
    mascara[1][1] = 0;
    mascara[1][2] = 0;

    mascara[2][0] = 1;
    mascara[2][1] = 2;
    mascara[2][2] = 1;

    return mascara;
}
//******************************------Gy------///////////////////////////////////
vector<vector<float>> gx() {

    vector<vector<float>> mascara(3, vector<float>(3, 0));

    mascara[0][0] = -1;
    mascara[0][1] = 0;
    mascara[0][2] = 1;

    mascara[1][0] = -2;
    mascara[1][1] = 0;
    mascara[1][2] = 2;

    mascara[2][0] = -1;
    mascara[2][1] = 0;
    mascara[2][2] = 1;

    return mascara;
}

float convolucion(Mat matrizBordes, vector<vector<float>> mascara, int mascSize, int x, int y) {

    int centroImg = (mascSize - 1) / 2; //centro de la imagen

    float sumatoriaFiltro = 0.0;

    for (int i = -centroImg; i <= centroImg; i++) {
        for (int j = -centroImg; j <= centroImg; j++) {

            float valMascara = mascara[i + centroImg][j + centroImg];
            int coordY = y + j + centroImg;
            int coordX = x + i + centroImg;

            float valImagen = matrizBordes.at<uchar>(coordY, coordX);

            sumatoriaFiltro += valMascara * valImagen;
        }
    }

    return sumatoriaFiltro;
}

Mat rellenar(int filas, int columnas, int Kernelsize)
{
    int diferenciaBordes = Kernelsize - 1;
    Mat matriz(filas + diferenciaBordes, columnas + diferenciaBordes, CV_8UC1);

    for (int i = 0; i < filas + diferenciaBordes; i++)
    {
        for (int j = 0; j < columnas + diferenciaBordes; j++)
        {
            matriz.at<uchar>(Point(j, i)) = uchar(0);
        }
    }

    return matriz;
}
//
Mat copiarRelleno(Mat bordes, Mat original, int Kernelsize)
{
    int diferenciaBordes = ((Kernelsize - 1) / 2);
    int filas = bordes.rows;
    int columnas = bordes.cols;

    for (int i = diferenciaBordes; i < filas - diferenciaBordes; i++)
    {
        for (int j = diferenciaBordes; j < columnas - diferenciaBordes; j++)
        {
            bordes.at<uchar>(Point(j, i)) = original.at<uchar>(Point(j - diferenciaBordes, i - diferenciaBordes));
        }
    }

    return bordes;
}
//Aplicar filtro a la imagen tomando los valores de la imagen, bordes, la mascara y el kernel
Mat aplicarFiltroImagen(Mat imagenOriginal, Mat matrizConBordes, vector<vector<float>> mascara, int kernel) {
    int filas = imagenOriginal.rows;
    int columnas = imagenOriginal.cols;

    Mat imagenFiltroAplicado(filas, columnas, CV_8UC1);

    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            float val = abs(static_cast<int>(convolucion(matrizConBordes, mascara, kernel, i, j)));
            imagenFiltroAplicado.at<uchar>(Point(i, j)) = val;
        }
    }

    return imagenFiltroAplicado;
}
/*
Mat sumaMatrices(Mat matriz1, Mat matriz2) {

    int filas = matriz1.rows;
    int columnas = matriz2.cols;

    Mat matrizResultante(filas, columnas, CV_8UC1);

    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            matrizResultante.at<uchar>(Point(i, j)) = matriz1.at<uchar>(Point(i, j)) + matriz2.at<uchar>(Point(i, j));
        }
    }

    return matrizResultante;
}
*/
/*
double Direccion(vector<vector<float>> Gx, vector<vector<float>> Gy) {

    double valX, valY, direccion;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            valX = Gx[i][j];
            valY = Gy[i][j];
            direccion = atan(valY / valX);
        }
    }

    return direccion;
}
*/
//////////////////////////Aplicacion de Sobel///////////////////////
//Se toma la mascara con la que derivamos para obtener un borde en este caso son Gx y Gy
//Gx: procesa las filas 
//Gy: procesa las Columnas 
Mat filtroSobel(Mat imagenGx, Mat imagenGy) {

    int filas = imagenGx.rows;
    int columnas = imagenGy.cols;

    Mat sobel(filas, columnas, CV_8UC1);

    int umbral = 125;
    double magnitud;
    double valGx, valGy;

    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {

            valGx = imagenGx.at<uchar>(Point(j, i));
            valGy = imagenGy.at<uchar>(Point(j, i));
            //Magnitud para obtener |G|
            magnitud = sqrt(pow(valGx, 2) + pow(valGy, 2));
            magnitud = static_cast<int>(magnitud);
            if (magnitud > umbral) {
                magnitud = 255;
            }
            else {
                magnitud = 0;
            }

            sobel.at<uchar>(Point(j, i)) = uchar(magnitud);
            //direccion = atan(valor_y / valor_x);
        }
    }


    return sobel;
}

////////////////////////////////---**Ecualización**---/////////////////////////////////

Mat ecualizacion(Mat imagenOriginal) {
    int rows = imagenOriginal.rows;
    int cols = imagenOriginal.cols;
    Mat ecualizada(rows, cols, CV_8UC1);


   

    double suma = 0;
    int c = 0;
    double valor = 0; 
    double max = 0, min = 10000;
    //la creamos en arreglo
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            valor = imagenOriginal.at<uchar>(Point(j, i));
            suma += imagenOriginal.at<uchar>(Point(j, i));
            
            if (valor < min && valor != 0) {
                min = valor;
            }
            if (valor > max) {
                max = valor;
            }
            c++;
        }
    }
    //obtener el maximo y minimo

    c = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            valor = imagenOriginal.at<uchar>(Point(j, i));
            ecualizada.at<uchar>(Point(j, i)) = (valor - min) * 255 / (max - min);
            c++;
        }
    }


    return ecualizada;
}

///////////////////////////////Funcion Principal////////////////////////////

int main()
{
    int mascSize = 0, filasImagen = 0, columnasImagen = 0;
    float sigma = 0.0;



    //Mat image;
    char imageName[] = "lena.png";
    Mat image = obtenerImagen(imageName);
    cout << "Digite el tamaño de la mascara (Kernel): ";
    cin >> mascSize;
    cout << "Digite el valor de 'sigma' para usar en el filtro Gaussiano: ";
    cin >> sigma;

    filasImagen = image.rows;
    columnasImagen = image.cols;

    namedWindow("Imagen Original a Color", WINDOW_AUTOSIZE);
    imshow("Imagen original a color", image);

    cvtColor(image, image, COLOR_BGR2GRAY);

    vector<vector<float>> mascGaussiana = mascaraKernerl(mascSize, sigma);
    vector<vector<float>> Gx = gx();
    vector<vector<float>> Gy = gy();

    Mat matrizConBordes = rellenar(filasImagen, columnasImagen, mascSize);
    matrizConBordes = copiarRelleno(matrizConBordes, image, mascSize);


    //Aplicamos el filtro gaussiano
    Mat imagenFiltroGaussiano = aplicarFiltroImagen(image, matrizConBordes, mascGaussiana, mascSize);


    

    ////Nuevamente creamos la expansion pero ahora la imagen del filtro gaussiano
    matrizConBordes = rellenar(filasImagen, columnasImagen, mascSize);
    matrizConBordes = copiarImgARelleno(matrizConBordes, imagenFiltroGaussiano, mascSize);

    //Aplicamos las mascaras de sobel 
    Mat imagenFiltroAplicadoGx = aplicarFiltroImagen(imagenFiltroGaussiano, matrizConBordes, Gx, 3);
    Mat imagenFiltroAplicadoGy = aplicarFiltroImagen(imagenFiltroGaussiano, matrizConBordes, Gy, 3);

    //Aplicamos la sumatoria del filtro
    Mat sobel = filtroSobel(imagenFiltroAplicadoGy, imagenFiltroAplicadoGx);

    Mat matrizEcualizada = ecualizacion(image);

    Mat CannyImage;
    Canny(matrizConBordes, CannyImage, 90, 150, 3, false);

    ////////////////////////--Imprimimos tamaños de imagenes--//////////////////////////////

    cout << endl << endl << "\t" << "Tamano imagen" << endl;

    cout << endl << "Imagen original" << "\t" << "\t" << "\t";
    cout << image.rows << "x";
    cout << image.cols << endl;

    cout << endl << "Imagen Imagen con bordes" << "\t";
    cout << matrizConBordes.rows << "x";
    cout << matrizConBordes.cols << endl;

    cout << endl << "Imagen con giltro Gaussiano" << "\t";
    cout << imagenFiltroGaussiano.rows << "x";
    cout << imagenFiltroGaussiano.cols << endl;

    cout << endl << "Imagen |G|" << "\t" << "\t" << "\t";
    cout << sobel.rows << "x";
    cout << sobel.cols << endl;

    cout << endl << "Imagen Ecualizada" << "\t" << "\t";
    cout << matrizEcualizada.rows << "x";
    cout << matrizEcualizada.cols << endl << endl << endl;

    /////////////////////////////--Imprimimos Pantalla--/////////////////////////////////////
    namedWindow("Imagen original", WINDOW_AUTOSIZE);
    imshow("Imagen original", image);
    ///*
    namedWindow("Imagen con filtro Gx", WINDOW_AUTOSIZE);
    imshow("Imagen con filtro Gx", imagenFiltroAplicadoGx);

    namedWindow("Imagen con filtro Gy", WINDOW_AUTOSIZE);
    imshow("Imagen con filtro Gy", imagenFiltroAplicadoGy);
    //*/

    namedWindow("Imagen con filtro G final", WINDOW_AUTOSIZE);
    imshow("Imagen con filtro G final", sobel);

    namedWindow("Imagen Bordes Extra", WINDOW_AUTOSIZE);
    imshow("Imagen Bordes Extra", matrizConBordes);

    namedWindow("Imagen Matriz Ecualizada ", WINDOW_AUTOSIZE);
    imshow("Imagen Matriz Ecualizada ", matrizEcualizada);

    namedWindow("Imagen Matriz CannyImage ", WINDOW_AUTOSIZE);
    imshow("Imagen Matriz CannyImage ", CannyImage);

    waitKey(0);

    return 1;
}