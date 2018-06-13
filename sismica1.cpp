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

//=========================== CABE�ALHO ==================================
#include "./sismica1.h"
#include <locale.h>
#include <cstdlib>

//========================= FUN��O MAIN ==================================
int main()
{
    setlocale(LC_ALL, "Portuguese");
    // Instanciando a classe Sintetico
    Sintetico a = Sintetico();

    // Carregando o arquivo de configura��o
    a.CarregarConfig();

    // Definindo os vetores de dados
    float impedancia[a.GetnumAmostra()] = {};
    float wavelet[a.GettamanhoWavelet()] = {};
    float refletividade[a.GetnumAmostra()] = {};
    float traco[a.GetnumAmostra()] = {};

    // Gerando a wavelet
    a.GeraWavelet(wavelet);

    // Gerando refletividade padr�o
    a.GeraRefletMod(refletividade,a.Getruido());

    // Gerando o tra�o padr�o
    a.convolucao(wavelet,refletividade,traco);

    if(a.Getpadrao())
    {
        // Exportar o tra�o padr�o
        for(int i = 0; i < a.GetnumTraco(); i++)
        {
            a.GravarTraco(traco, i);
        }
    }else
    {
        for(int i = 0; i < a.GetnumTraco(); i++)
        {
            // Atualizando a refletividade
            a.GeraRefletImped(impedancia, refletividade);

            // Atualizando o(s) tra�o(s) sint�tico(s)
            a.convolucao(wavelet,refletividade,traco);

            // Exportar o tra�o atualizado
            a.GravarTraco(traco, i);
        }
    }
    system("PAUSE");
    return 0;
}

//============ IMPLEMENTA��O DOS M�TODOS DA CLASSE SINTETICO =============
void Sintetico::GeraWavelet(float wavelet[])
{
    // Fator da fun��o Ricker.
    float a = -1.*pow(pi*GetfreqPico(),2);

    // Per�odo da frequ�ncia de pico.
    float periodoPico = 1./GetfreqPico();

    // Calculando o vetor wavelet.
    for (int i = 0; i < GettamanhoWavelet(); i++)
    {
        wavelet[i] = (float) Getpolaridade()*(((1 + 2*a*pow(
                    ((Getamostragem()*i)-periodoPico),2))*exp(a*pow(
                    ((Getamostragem()*i)-periodoPico),2))));
    }
}
//------------------------------------------------------------------------

void Sintetico::GeraRefletImped(float impedancia[], float refletividade[])
{
    // Calculando a refletividade, exceto para a �ltima amostra.
    for (int i = 0; i < GetnumAmostra() - 1; i++)
    {
        refletividade[i] = (impedancia[i+1] - impedancia[i])/
                           (impedancia[i+1] + impedancia[i]);
    }

    // �ltima amostra.
    refletividade[GetnumAmostra()-1] = refletividade[GetnumAmostra()-2];
}
//------------------------------------------------------------------------

void Sintetico::GeraRefletMod(float refletividade[], bool ruido)
{
    // Zerando as amostras.
    for(int n = 0; n < GetnumAmostra(); n++)
    {
        refletividade[n] = 0.;
    }

    // Incluindo ruido de, no m�ximo, 0.05.
    if(ruido)
    {
        int aux = GetnumAmostra();
        for(int m = 0; m < (GetnumAmostra()/4); m++)
        {
            refletividade[rand()%aux] = pow(-1,rand()%2)*(1.0*
                                        (rand()%6)/100.);
        }
    }

    // Calculando 5 refletividades aleat�rias, com mesmo espa�amento e com
    // valor absoluto maior ou igual 0.3 e menor que 1.
    float passo = (1.0*GetnumAmostra())/6.;
    int indice = 0;
    for(int p = 1; p < 6; p++)
    {
        indice = passo*p;
        refletividade[indice] = pow(-1,rand()%2)*((0.699*
                                (rand()%11)/10.)+0.3);
    }
}
//------------------------------------------------------------------------

void Sintetico::convolucao(float wavelet[], float refletividade[], float traco[])
{
    // Vetor auxiliar
    float aux[GetnumAmostra() + GettamanhoWavelet() - 1]={};

    // Convolu��o linear completa
    for(int i = 0; i < GetnumAmostra(); i++)
    {
        for(int j = 0; j < GettamanhoWavelet(); j++)
        {
            aux[i+j] = aux[i+j] + wavelet[j]*refletividade[i];
        }
    }

    // Tamanho a ser removido do in�cio e do fim do tra�o
    int tamanho =  GettamanhoWavelet()/2;

    // Removendo trechos do tra�o
    for(int j = tamanho; j < tamanho + GetnumAmostra(); j++)
    {
        traco[j-tamanho] = aux[j];
    }
}
//------------------------------------------------------------------------

void Sintetico::CarregarConfig()
{
    // Controle de tela
    cout << "CARREGANDO ARQUIVO DE CONFIGURA��O. \n\n\n";

    // Endere�o do arquivo de configura��o - mesmo local do execut�vel.
    string enderecoConfig = "./sismica1.ini";

    // Verifica se o arquivo foi aberto com sucesso.
    ifstream ifpConfig(enderecoConfig);

    // Contador para os atributos
    int contParametro = 0;

	string linha;
    if (ifpConfig)
    {
        // Controle de tela
        cout << "=========================================================================" << "\n";
        cout << "                 ARQUIVO DE CONFIGURA��O ENCONTRADO                      " << "\n";
        cout << "=========================================================================" << "\n";


        // Enquanto n�o chegar o fim do arquivo fa�a:
        while (!ifpConfig.eof())
        {
            // Leia a linha atual do arquivo
            getline(ifpConfig,linha);

            // Se a linha n�o for vazia ou um coment�rio fa�a:
            if (!(linha.length() == 0 || linha[0] == '/'))
            {
                // Carrega o endere�o do arquivo de imped�ncia
                if(contParametro == id_enderecoimp)
                {
                    SetenderecoImp(linha);

                }

                // Carrega o n�mero de tra�os
                if(contParametro == id_numtraco)
                {
                    // atoi converte de string para int
                    if(atoi(linha.c_str())>0)
                        SetnumTraco(atoi(linha.c_str()));
                    else
                        SetnumTraco(1);
                }

                // Carrega o n�mero de amostras
                if(contParametro == id_numamostra)
                {
                    if(atoi(linha.c_str()) >= 0)
                        SetnumAmostra(atoi(linha.c_str()));
                    else
                        SetnumAmostra(1000);
                }

                // Carrega a taxa de amostragem
                if(contParametro == id_amostragem)
                {
                    // atof converte de string para double
                    if (atof(linha.c_str()) >= 0)
                        Setamostragem(atof(linha.c_str()));
                    else
                        Setamostragem(0.004);
                }

                // Carrega a frequ�ncia de pico
                if(contParametro == id_freqpico)
                {
                    if (atof(linha.c_str()) >= 0)
                        SetfreqPico(atof(linha.c_str()));
                    else
                        SetfreqPico(20);
                }

                // Carrega a polaridade da wavelet
                if(contParametro == id_polaridade)
                {
                    if ((atof(linha.c_str()) >= 0)&&(atof(linha.c_str()) <= 1))
                        Setpolaridade((short) atof(linha.c_str()));
                    else
                        Setpolaridade(1);
                }

                // Carrega a presen�a de ru�do
                if(contParametro == id_ruido)
                {
                    if ((atof(linha.c_str()) >= 0)&&(atof(linha.c_str()) <= 1))
                        Setruido((bool) atof(linha.c_str()));
                    else
                        Setruido(0);
                }

                // Define se usa o modelo padr�o de refletividade
                if(contParametro == id_padrao)
                {
                    if ((atof(linha.c_str()) >= 0)&&(atof(linha.c_str()) <= 1))
                        Setpadrao((bool) atof(linha.c_str()));
                    else
                        Setpadrao(1);
                }
                contParametro++;

            }

        }
    }else
    {
        // Controle de tela
        cout << "=========================================================================" << "\n";
        cout << "                 ARQUIVO DE CONFIGURA��O N�O ENCONTRADO                  " << "\n";
        cout << "=========================================================================" << "\n";
        cout << "Usando a configura��o padr�o, conforme abaixo:" << "\n";

        SetenderecoImp(" ");
        SetnumTraco(1);
        SetnumAmostra(1000);
        Setamostragem(0.004);
        SetfreqPico(20);
        Setpolaridade(1);
        Setruido(0);
        Setpadrao(1);
    }

    // Definindo tamanho da wavelet
    SettamanhoWavelet((int)(2./(GetfreqPico()*Getamostragem()))+2);

    // Controle de tela
    cout << "\n";
    cout << "Arquivo de Imped�ncia                                             " << GetenderecoImp() << "\n";
    cout << "N�mero de tra�os                                                  " << GetnumTraco() << "\n";
    cout << "N�mero de amostras                                                " << GetnumAmostra() << "\n";
    cout << "Taxa de amostragem                                                " << Getamostragem() << "\n";
    cout << "Frequ�ncia de pico da wavelet                                     " << GetfreqPico() << "\n";
    cout << "Polaridade                                                        " << Getpolaridade() << "\n";
    cout << "Usa ru�do?                                                        " << (Getruido() == 1 ? "SIM\n" : "N�O\n");
    cout << "Usa modelo padr�o de refletividade?                               " << (Getpadrao() == 1 ? "SIM\n" : "N�O\n");
    cout << "Tamanho da wavelet                                                " <<  GettamanhoWavelet() << "\n";
    cout << "\n";

    // Fechando o arquivo de configura��o
    ifpConfig.close();
}
//------------------------------------------------------------------------

// N�O IMPLEMENTADO. � NECESS�RIO CONHECER O ARQUIVO DE IMPED�NCIA
void Sintetico::CarregarImped(float impedancia[])
{

}
//------------------------------------------------------------------------

void Sintetico::GravarTraco(float traco[], int indice)
{
    // Abrindo os arquivos de entrada e tempor�rio de sa�da
    ifstream entrada("./Tracos.txt");
    FILE *saida = fopen ("./Tracos_tmp.txt", "w+");

    // Linha do arquivo
    string linha;

    // Contador para as amostras do tra�o
    int cont = 0;

    // Se n�o for o primeiro tra�o, recupera os dados do arquivo
    if(indice)
    {
        while (!entrada.eof())
        {
            // L� a linha atual do arquivo entrada
            getline(entrada,linha);

            if (!(linha.length() == 0 || linha[0] == '/'))
            {
                // Concatena as informa��es do arquivo com o tra�o atual
                fprintf(saida,"%s",linha.c_str());
                fprintf(saida,"%3.2f",traco[cont]);
                fprintf(saida,"\t\n","");
                cont++;
            }
        }

    // Se for o primeiro tra�o, descarta o arquivo
    }else
    {
        for (int i = 0; i < GetnumAmostra(); i++)
        {
            fprintf(saida,"%3.2f",traco[cont]);
            fprintf(saida,"\t\n","");
            cont++;
        }
    }

    // Fechando os arquivos
    fclose(saida);
    entrada.close();

    // Excluindo o arquivo antigo
    remove("./Tracos.txt");
    rename("./Tracos_tmp.txt","./Tracos.txt");
}
