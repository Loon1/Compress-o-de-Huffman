#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>


#define Fim_Simbolo       256
#define Especial          257
#define Contador          258
#define Tamanho_tabela    ( ( Contador * 2 ) - 1 )
#define NoRaiz            0
#define Max               0x8000
#define TRUE              1
#define FALSE             0
#define PCONT             2047

using namespace std;

typedef struct bit_arquivo {
    FILE *arquivo;
    unsigned char maux;
    int raux;
    int p_contador;
} BIT_ARQUIVO;

void erro_fatal( char *fmt, ... )
{
    va_list argptr;

    va_start( argptr, fmt );
    printf( "Erro: " );
    vprintf( fmt, argptr );
    va_end( argptr );
    exit( -1 );
}

BIT_ARQUIVO *AbrirArquivoBitEntrada(char *nome)
{
    BIT_ARQUIVO *bit_arquivo;

    bit_arquivo = (BIT_ARQUIVO *) calloc( 1, sizeof( BIT_ARQUIVO ) );
    if ( bit_arquivo == NULL )
        return( bit_arquivo );
    bit_arquivo->arquivo = fopen( nome, "rb" );
    bit_arquivo->raux = 0;
    bit_arquivo->maux = 0x80;
    bit_arquivo->p_contador = 0;
    return( bit_arquivo );
}

void FecharArquivoBitSaida(BIT_ARQUIVO *bit_arquivo)
{
    if ( bit_arquivo->maux != 0x80 )
        if ( putc( bit_arquivo->raux, bit_arquivo->arquivo ) != bit_arquivo->raux )
            erro_fatal( "Erro  em FecharArquivoBit!\n" );
    fclose( bit_arquivo->arquivo );
    free( (char *) bit_arquivo );
}

void FecharArquivoBitEntrada(BIT_ARQUIVO *bit_arquivo)
{
    fclose( bit_arquivo->arquivo );
    free( (char *) bit_arquivo );
}

void SaidaBit(BIT_ARQUIVO *bit_arquivo,int bit)
{
    if ( bit )
        bit_arquivo->raux |= bit_arquivo->maux;
    bit_arquivo->maux >>= 1;
    if ( bit_arquivo->maux == 0 ) {
        if ( putc( bit_arquivo->raux, bit_arquivo->arquivo ) != bit_arquivo->raux )
            erro_fatal( "Erro em SaidaBit!\n" );
        else
            if ( ( bit_arquivo->p_contador++ & PCONT ) == 0 )
                ;
        bit_arquivo->raux = 0;
        bit_arquivo->maux = 0x80;
    }
}

BIT_ARQUIVO *AbrirArquivoBitSaida(char *nome)
{
    BIT_ARQUIVO *bit_arquivo;

    bit_arquivo = (BIT_ARQUIVO *) calloc( 1, sizeof( BIT_ARQUIVO ) );
    if ( bit_arquivo == NULL )
        return( bit_arquivo );
    bit_arquivo->arquivo = fopen( nome, "wb" );
    bit_arquivo->raux = 0;
    bit_arquivo->maux = 0x80;
    bit_arquivo->p_contador = 0;
    return( bit_arquivo );
}

void SaidaBits(BIT_ARQUIVO *bit_arquivo, unsigned long code, int count)
{
    unsigned long maux;

    maux = 1L << ( count - 1 );
    while ( maux != 0) {
        if ( maux & code )
            bit_arquivo->raux |= bit_arquivo->maux;
        bit_arquivo->maux >>= 1;
        if ( bit_arquivo->maux == 0 ) {
            if ( putc( bit_arquivo->raux, bit_arquivo->arquivo ) != bit_arquivo->raux )
                erro_fatal( "Erro em SaidaBit!\n" );
            else
                if ( ( bit_arquivo->p_contador++ & PCONT ) == 0 )
                    ;
            bit_arquivo->raux = 0;
            bit_arquivo->maux = 0x80;
        }
        maux >>= 1;
    }
}

int EntradaBit(BIT_ARQUIVO *bit_arquivo)
{
    int value;

    if ( bit_arquivo->maux == 0x80 ) {
        bit_arquivo->raux = getc( bit_arquivo->arquivo );
        if ( bit_arquivo->raux == EOF )
            erro_fatal( "Erro fatal em EntradaBit!\n" );
        bit_arquivo->p_contador++;
    }
    value = bit_arquivo->raux & bit_arquivo->maux;
    bit_arquivo->maux >>= 1;
    if ( bit_arquivo->maux == 0 )
        bit_arquivo->maux = 0x80;
    return( value ? 1 : 0 );
}

unsigned long EntradaBits(BIT_ARQUIVO *bit_arquivo, int bit_count)
{
    unsigned long maux;
    unsigned long return_value;

    maux = 1L << ( bit_count - 1 );
    return_value = 0;
    while ( maux != 0) {
        if ( bit_arquivo->maux == 0x80 ) {
            bit_arquivo->raux = getc( bit_arquivo->arquivo );
            if ( bit_arquivo->raux == EOF )
                erro_fatal( "Erro em EntradaBit!\n" );
            bit_arquivo->p_contador++;
        }
        if ( bit_arquivo->raux & bit_arquivo->maux )
            return_value |= maux;
        maux >>= 1;
        bit_arquivo->maux >>= 1;
        if ( bit_arquivo->maux == 0 )
            bit_arquivo->maux = 0x80;
    }
    return( return_value );
}

void ImprimirBinarioArquivo(FILE *arquivo, unsigned int code, int bits)
{
    unsigned int maux;

    maux = 1 << ( bits - 1 );
    while ( maux != 0 ) {
        if ( code & maux )
            fputc( '1', arquivo );
        else
            fputc( '0', arquivo );
        maux >>= 1;
    }
}

typedef struct no {
    unsigned int peso;
    int pai;
    int filho_e_folha;
    int filho;
} no;

typedef struct arvore {
    int folha[Contador];
    int proximo_no_livre;
    no nos[Tamanho_tabela];
} ARVORE;

ARVORE Arvore;

void InicializarArvore(ARVORE *arvore)
{
    int i;

    arvore->nos[ NoRaiz ].filho             = NoRaiz + 1;
    arvore->nos[ NoRaiz ].filho_e_folha     = FALSE;
    arvore->nos[ NoRaiz ].peso              = 2;
    arvore->nos[ NoRaiz ].pai               = -1;

    arvore->nos[ NoRaiz + 1 ].filho         = Fim_Simbolo;
    arvore->nos[ NoRaiz + 1 ].filho_e_folha = TRUE;
    arvore->nos[ NoRaiz + 1 ].peso          = 1;
    arvore->nos[ NoRaiz + 1 ].pai           = NoRaiz;
    arvore->folha[ Fim_Simbolo ]            = NoRaiz + 1;

    arvore->nos[ NoRaiz + 2 ].filho         = Especial;
    arvore->nos[ NoRaiz + 2 ].filho_e_folha = TRUE;
    arvore->nos[ NoRaiz + 2 ].peso          = 1;
    arvore->nos[ NoRaiz + 2 ].pai           = NoRaiz;
    arvore->folha[ Especial ]               = NoRaiz + 2;

    arvore->proximo_no_livre                = NoRaiz + 3;

    for ( i = 0 ; i < Fim_Simbolo ; i++ )
        arvore->folha[ i ] = -1;
}

void trocar_nos(ARVORE *arvore, int i, int j)
{
    struct no temp;

    if ( arvore->nos[ i ].filho_e_folha )
        arvore->folha[ arvore->nos[ i ].filho ] = j;
    else {
        arvore->nos[ arvore->nos[ i ].filho ].pai = j;
        arvore->nos[ arvore->nos[ i ].filho + 1 ].pai = j;
    }
    if ( arvore->nos[ j ].filho_e_folha )
        arvore->folha[ arvore->nos[ j ].filho ] = i;
    else {
        arvore->nos[ arvore->nos[ j ].filho ].pai = i;
        arvore->nos[ arvore->nos[ j ].filho + 1 ].pai = i;
    }
    temp = arvore->nos[ i ];
    arvore->nos[ i ] = arvore->nos[ j ];
    arvore->nos[ i ].pai = temp.pai;
    temp.pai = arvore->nos[ j ].pai;
    arvore->nos[ j ] = temp;
}

void adicionar_novo_no(ARVORE *arvore, int c)
{
    int no_mais_leve;
    int novo_no;
    int no_com_peso_zero;

    no_mais_leve = arvore->proximo_no_livre - 1;
    novo_no = arvore->proximo_no_livre;
    no_com_peso_zero = arvore->proximo_no_livre + 1;
    arvore->proximo_no_livre += 2;

    arvore->nos[ novo_no ] = arvore->nos[ no_mais_leve ];
    arvore->nos[ novo_no ].pai = no_mais_leve;
    arvore->folha[ arvore->nos[ novo_no ].filho ] = novo_no;

    arvore->nos[ no_mais_leve ].filho         = novo_no;
    arvore->nos[ no_mais_leve ].filho_e_folha = FALSE;

    arvore->nos[ no_com_peso_zero ].filho           = c;
    arvore->nos[ no_com_peso_zero ].filho_e_folha   = TRUE;
    arvore->nos[ no_com_peso_zero ].peso            = 0;
    arvore->nos[ no_com_peso_zero ].pai             = no_mais_leve;
    arvore->folha[ c ] = no_com_peso_zero;
}

int DecodificarSimbolo(ARVORE *arvore, BIT_ARQUIVO *entrada)
{
    int no_atual;
    int c;

    no_atual = NoRaiz;
    while ( !arvore->nos[ no_atual ].filho_e_folha ) {
        no_atual = arvore->nos[ no_atual ].filho;
        no_atual += EntradaBit( entrada );
    }
    c = arvore->nos[ no_atual ].filho;
    if ( c == Especial ) {
        c = (int) EntradaBits( entrada, 8 );
        adicionar_novo_no( arvore, c );
    }
    return( c );
}

void ReconstruirArvore(ARVORE *arvore)
{
    int i;
    int j;
    int k;
    unsigned int peso;


    j = arvore->proximo_no_livre - 1;
    for ( i = j ; i >= NoRaiz ; i-- ) {
        if ( arvore->nos[ i ].filho_e_folha ) {
            arvore->nos[ j ] = arvore->nos[ i ];
            arvore->nos[ j ].peso = ( arvore->nos[ j ].peso + 1 ) / 2;
            j--;
        }
    }


    for ( i = arvore->proximo_no_livre - 2 ; j >= NoRaiz ; i -= 2, j-- ) {
        k = i + 1;
        arvore->nos[ j ].peso = arvore->nos[ i ].peso +
                                  arvore->nos[ k ].peso;
        peso = arvore->nos[ j ].peso;
        arvore->nos[ j ].filho_e_folha = FALSE;
        for ( k = j + 1 ; peso < arvore->nos[ k ].peso ; k++ )
            ;
        k--;
        memmove( &arvore->nos[ j ], &arvore->nos[ j + 1 ],
                 ( k - j ) * sizeof( struct no ) );
        arvore->nos[ k ].peso = peso;
        arvore->nos[ k ].filho = i;
        arvore->nos[ k ].filho_e_folha = FALSE;
    }

    for ( i = arvore->proximo_no_livre - 1 ; i >= NoRaiz ; i-- ) {
        if ( arvore->nos[ i ].filho_e_folha ) {
            k = arvore->nos[ i ].filho;
            arvore->folha[ k ] = i;
        } else {
            k = arvore->nos[ i ].filho;
            arvore->nos[ k ].pai = arvore->nos[ k + 1 ].pai = i;
        }
    }
}

void AtualizarModelo(ARVORE *arvore, int c)
{
    int no_atual;
    int novo_no;

    if ( arvore->nos[ NoRaiz].peso == Max )
        ReconstruirArvore( arvore );
    no_atual = arvore->folha[ c ];
    while ( no_atual != -1 ) {
        arvore->nos[ no_atual ].peso++;
        for ( novo_no = no_atual ; novo_no > NoRaiz ; novo_no-- )
            if ( arvore->nos[ novo_no - 1 ].peso >=
                 arvore->nos[ no_atual ].peso )
                break;
        if ( no_atual != novo_no ) {
            trocar_nos( arvore, no_atual, novo_no );
            no_atual = novo_no;
        }
        no_atual = arvore->nos[ no_atual ].pai;
    }
}

void CodificarSimbolo(ARVORE *arvore,unsigned int c,BIT_ARQUIVO *saida)
{
    unsigned long code;
    unsigned long maux;
    int tam_codigo;
    int no_atual;

    code = 0;
    maux = 1;
    tam_codigo = 0;
    no_atual = arvore->folha[ c ];
    if ( no_atual == -1 )
        no_atual = arvore->folha[ Especial ];
    while ( no_atual != NoRaiz ) {
        if ( ( no_atual & 1 ) == 0 )
            code |= maux;
        maux <<= 1;
        tam_codigo++;
        no_atual = arvore->nos[ no_atual ].pai;
    };
    SaidaBits( saida, code, tam_codigo );
    if ( arvore->folha[ c ] == -1 ) {
        SaidaBits( saida, (unsigned long) c, 8 );
        adicionar_novo_no( arvore, c );
    }
}

void ComprimirArquivoHuffmanAdaptativo(FILE *entrada,BIT_ARQUIVO *saida)
{
    int c;

    
    InicializarArvore( &Arvore );
    while ( ( c = getc( entrada ) ) != EOF ) {
        CodificarSimbolo( &Arvore, c, saida );
        AtualizarModelo( &Arvore, c );
    }
    CodificarSimbolo( &Arvore, Fim_Simbolo, saida );
}

void ExpandirArquivoHuffmanAdaptativo(BIT_ARQUIVO *entrada,FILE *saida)
{
    int c;

    
    InicializarArvore( &Arvore );
    while ( ( c = DecodificarSimbolo( &Arvore, entrada ) ) != Fim_Simbolo ) {
        if ( putc( c, saida ) == EOF );
        AtualizarModelo( &Arvore, c );
    }
}

void uso_saida(char *nome_prog)
{
    char *nome_curto;
    char *extensao;

    nome_curto = strrchr( nome_prog, '\\' );
    if ( nome_curto == NULL )
        nome_curto = strrchr( nome_prog, '/' );
    if ( nome_curto == NULL )
        nome_curto = strrchr( nome_prog, ':' );
    if ( nome_curto != NULL )
        nome_curto++;
    else
        nome_curto = nome_prog;
    extensao = strrchr( nome_curto, '.' );
    if ( extensao != NULL )
        *extensao = '\0';
    printf( "\nUso:  %s arquivo-entrada\n", nome_curto);
    exit( 0 );
}

long tamanho_arquivo(char *nome)
{
    long posicao_final;
    FILE *arquivo;

    arquivo = fopen( nome, "r" );
    if ( arquivo == NULL )
        return( 0L );
    fseek( arquivo, 0L, SEEK_END );
    posicao_final = ftell( arquivo );
    fclose( arquivo );
    return( posicao_final );
}

char *gerar_nome_temporario()
{
    static char nome_temporario[100];
    const char caracteres_validos[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const size_t tamanho_caracteres_validos = strlen(caracteres_validos);
    srand((unsigned int)time(NULL));


    for (int i = 0; i < 6; i++)
    {
        nome_temporario[i] = caracteres_validos[rand() % tamanho_caracteres_validos];
    }

    nome_temporario[6] = '\0';
    strcat(nome_temporario, ".out");

    return nome_temporario;
}

int main(int argc, char *argv[])
{
    BIT_ARQUIVO *entrada;
    FILE *saida;

    setbuf(stdout, NULL);
    if (argc < 2)
        uso_saida(argv[0]);

    char *nome_temporario = gerar_nome_temporario();

    entrada = AbrirArquivoBitEntrada(argv[1]);
    if (entrada == NULL)
    {
        erro_fatal("Erro ao abrir o arquivo %s\n", argv[1]);
    }

    saida = fopen(nome_temporario, "wb");
    if (saida == NULL)
    {
        FecharArquivoBitEntrada(entrada);
        erro_fatal("Erro ao abrir %s para saída\n", nome_temporario);
    }

    printf("Descompressão de Huffman Adaptativo\n");
    ExpandirArquivoHuffmanAdaptativo(entrada, saida);

    FecharArquivoBitEntrada(entrada);
    fclose(saida);

    return 0;
}
