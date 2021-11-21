#include <iostream>
#include <random>
#include <pthread.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
//Funcao generica para a criacao de um double randomico
double fRand(double fMin, double fMax) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(fMin, fMax);
    return dist(mt);
}

//Geramos um cosseno randomico
double sample_cosine() {
    double rc = 2;
    double u1 = fRand(0,1);
    double u2 = fRand(0,1);
    double v1 = 2 * u1 - 1;
    double rr = (v1 * v1) + (u2 * u2);
    while (rr > 1) {
        u1 = fRand(0,1);
        u2 = fRand(0,1);
        v1 = 2 * u1 - 1;
        rr = (v1 * v1) + (u2 * u2);
    }
    double cc = (v1 * v1 - u2 * u2)/rr;
    return cc;
}

//Definimos o tamanho da nosso plano
const double x = 1;
const double y = 2;
const double p = x/y;

//Definimos o numero de palitos e de simulacoes a rodar
const int n = 80000;
const int m = 4;
double pi_approx[m];

//Realiza uma simulacao
void *SimulateInAThread(void *thread_id) {
    int tid;
    tid = (long)thread_id;

    std::vector <double> r (n);
    std::vector <double> z (m);

    //Joga as agulhas na caixa
    for (size_t i = 0; i < n; i++) {
        r[i] = fRand(0,y);
        double c = sample_cosine();
        z[i] = (c*x)/2;
    }
    double hits = 0;
    for (size_t i = 0; i < n; i++) {
        //Identificacao de um hit
        if ((r[i] + z[i]) >= y || (r[i] - z[i]) < 0)
        {
            hits++;
        }
    }
    double est = p * n / hits;
    std::cout << "A thread " << tid; 
    std::cout << " Deu o valor estimado de " << est << "\n";
    
    pthread_mutex_lock ( &mutex1);
    pi_approx[tid] = est;
    pthread_mutex_unlock ( &mutex1);

}

int main() { 
    pthread_t threads[m];
    int rc;
    int i;
    double piSum;

    //Criamos cada simulacao em uma thread separada
    for (i = 0; i < m; i++) {
        std::cout << "main() Criando a thread: " << i << "\n";
        rc = pthread_create(&threads[i], NULL, SimulateInAThread, (void *)i);
        if (rc) {
         std::cout << "Error:unable to create thread," << rc << std::endl;
         exit(-1);
        }
    }
    //Esperamos todas as threads acabarem para printar o resultado
    for (i = 0; i < m; i++) {
        pthread_join( threads[i], NULL);
    }
    //Podemos assumir que neste ponto todas as simulacoes ja acabaram, entao vamos tirar o valor medio de todas para obter um valor de pi
    for (size_t j = 0; j < m; j++)
    {
        piSum += pi_approx[j];
    }
    double piAverage = piSum/m;
    std::cout << "A media de todas as simulacoes de pi eh " << piAverage << std::endl;

    exit(0);

}




