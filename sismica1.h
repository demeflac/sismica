//////////////////////////////////////////////////////////////////////////
// PROGRAMA: sismica1.h                                                  /
//                                                                       /
//   Descri��o: Calcula as amostras de um tra�o s�smico sint�tico usando /
//              convolu��o.                                              /
//                                                                       /
// Autor: Carlos da Silva Claudino                                       /
// Data:07/05/2018                                                       /
//                                                                       /
//////////////////////////////////////////////////////////////////////////

//========================== CABE�ALHOS ==================================
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <limits>
#include "math.h"

//============================ MACROS ====================================
//Macros para manipula��o dos atributos da classe. Fun��es get e set.
#define GetSet(type, name)\
private: type _##name; \
public: \
void Set##name(const type& var) \
{\
        _##name = var;\
}\
\
type Get##name() \
{\
        return _##name; \
}\

//=============== DECLARA��O DA CLASSE SINTETICO =========================
using namespace std;

class Sintetico
{
    public:

        //======================= CONSTRUTOR =============================

        /**< Construtor. */
        Sintetico(){};

        /**< Destrutor */
        ~Sintetico(){};

        //========================= M�TODOS ==============================
        /**< Gera uma wavelet. */
        void GeraWavelet(float wavelet[]);

        /**< Gera refletividade a partir da imped�ncia. */
        void GeraRefletImped(float impedancia[], float refletividade[]);

        /**< Gera um exemplo de refletividade, se ruido for "true", a
             refletividade ser� gerada com ru�do. */
        void GeraRefletMod(float refletividade[], bool ruido);

        /**< Realiza a convolu��o linear entre dois vetores. */
        void convolucao(float wavelet[], float refletividade[], float traco[]);

        /**< Ler arquivo de configura��o. */
        void CarregarConfig();

        /**< Ler arquivo de imped�ncia. */
        void CarregarImped(float impedancia[]);

        /**< Gravar o tra�o em um arquivo texto. */
        void GravarTraco(float traco[], int indice);

        //----------------------------------------------------------------

        /* IDENTIFICADORES - Usados para identificar os atributos no
        arquivo de configura��o. N�o altere a sequ�ncia abaixo. Se houver
        necessidade de inclus�o de algum atributo, coloque seu identificador
        no fim da lista e atualize o arquivo "sismica.ini" e o
        m�todo "CarregaConfig".*/

        enum
        {
            id_enderecoimp,
            id_numtraco,
            id_numamostra,
            id_amostragem,
            id_freqpico,
            id_polaridade,
            id_ruido,
            id_padrao
        };
        //----------------------------------------------------------------

    private:

        //========================= ATRIBUTOS ============================
        // CONSTANTES
        /**< Valor de pi. */
        const double pi = 3.141592653589793238460;
        //----------------------------------------------------------------

        /*DECLARA��O - Os comandos abaixo declaram os atributos e
        implementam suas fun��es get e set. */

        /**< Endere�o do arquivo de imped�ncia. */
        GetSet(string, enderecoImp);

        /**< N�mero de tra�os. */
        GetSet(long, numTraco);

        /**< N�mero de amostras. */
        GetSet(long, numAmostra);

        /**< Taxa de amostragem. */
        GetSet(float, amostragem);

        /**< Frequ�ncia de pico da wavelet. */
        GetSet(float, freqPico);

        /**< Tamanho da wavelet. */
        GetSet(int, tamanhoWavelet);

        /**< Polaridade da Wavelet. */
        GetSet(short, polaridade);

        /**< Presen�a ou n�o de ru�do */
        GetSet(bool, ruido);

        /**< Usa ou n�o a refletividade padrao */
        GetSet(bool, padrao);
        //----------------------------------------------------------------
};
