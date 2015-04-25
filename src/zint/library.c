/*  library.c - external functions of libzint

    libzint - the open source barcode library
    Copyright (C) 2009 Robin Stuart <robin@zint.org.uk>

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the project nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef _MSC_VER
#include <malloc.h>
#endif
#include "common.h"

#define TECHNETIUM	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-. $/+%"

struct zint_symbol *ZBarcode_Create()
{
    struct zint_symbol *symbol;
    int i;

    symbol = (struct zint_symbol*)malloc(sizeof(*symbol));
    if (!symbol) return NULL;

    memset(symbol, 0, sizeof(*symbol));
    symbol->symbology = BARCODE_CODE128;
    symbol->height = 0;
    symbol->whitespace_width = 0;
    symbol->border_width = 0;
    symbol->output_options = 0;
    symbol->rows = 0;
    symbol->width = 0;
    strcpy(symbol->fgcolour, "000000");
    strcpy(symbol->bgcolour, "ffffff");
    strcpy(symbol->outfile, "");
    symbol->scale = 1.0;
    symbol->option_1 = -1;
    symbol->option_2 = 0;
    symbol->option_3 = 928; // PDF_MAX
    symbol->show_hrt = 1; // Show human readable text
    symbol->input_mode = DATA_MODE;
    strcpy(symbol->primary, "");
    memset(&(symbol->encoded_data[0][0]),0,sizeof(symbol->encoded_data));
    for(i = 0; i < 178; i++) {
                symbol->row_height[i] = 0;
        }
    symbol->bitmap = NULL;
    symbol->bitmap_width = 0;
    symbol->bitmap_height = 0;
    return symbol;
}

void ZBarcode_Clear(struct zint_symbol *symbol)
{
    int i, j;

    for(i = 0; i < symbol->rows; i++) {
        for(j = 0; j < symbol->width; j++) {
            unset_module(symbol, i, j);
        }
    }
    symbol->rows = 0;
    symbol->width = 0;
    symbol->text[0] = '\0';
    symbol->errtxt[0] = '\0';
    if (symbol->bitmap != NULL)
    {
        free(symbol->bitmap);
        symbol->bitmap = NULL;
    }
    symbol->bitmap_width = 0;
    symbol->bitmap_height = 0;
}

void ZBarcode_Delete(struct zint_symbol *symbol)
{
    if (symbol->bitmap != NULL)
        free(symbol->bitmap);

    // If there is a rendered version, ensure its memory is released
    if (symbol->rendered != NULL) {
        struct zint_render_line *line, *l;
        struct zint_render_string *string, *s;

        // Free lines
        line = symbol->rendered->lines;
        while(line) {
            l = line;
            line = line->next;
            free(l);
        }
        // Free Strings
        string = symbol->rendered->strings;
        while (string) {
            s = string;
            string = string->next;
            free(s->text);
            free(s);
        }

        // Free Render
        free(symbol->rendered);
    }
    free(symbol);
}

extern int pdf417enc(struct zint_symbol *symbol, unsigned char source[], int length); /* PDF417 */
extern int qr_code(struct zint_symbol *symbol, unsigned char source[], int length); /* QR Code */
extern int aztec(struct zint_symbol *symbol, unsigned char source[], int length); /* Aztec Code */

extern int bmp_handle(struct zint_symbol *symbol, int rotate_angle);

void error_tag(char error_string[], int error_number)
{
    char error_buffer[100];

    if(error_number != 0) {
        strcpy(error_buffer, error_string);

        if(error_number > 4) {
            strcpy(error_string, "error: ");
        } else {
            strcpy(error_string, "warning: ");
        }

        concat(error_string, error_buffer);
    }
}

int ZBarcode_ValidID(int symbol_id)
{
    /* Checks whether a symbology is supported */

    int result = 0;

    switch(symbol_id) {
        case BARCODE_CODE11:
        case BARCODE_C25MATRIX:
        case BARCODE_C25INTER:
        case BARCODE_C25IATA:
        case BARCODE_C25LOGIC:
        case BARCODE_C25IND:
        case BARCODE_CODE39:
        case BARCODE_EXCODE39:
        case BARCODE_EANX:
        case BARCODE_EAN128:
        case BARCODE_CODABAR:
        case BARCODE_CODE128:
        case BARCODE_DPLEIT:
        case BARCODE_DPIDENT:
        case BARCODE_CODE16K:
        case BARCODE_CODE49:
        case BARCODE_CODE93:
        case BARCODE_FLAT:
        case BARCODE_RSS14:
        case BARCODE_RSS_LTD:
        case BARCODE_RSS_EXP:
        case BARCODE_TELEPEN:
        case BARCODE_UPCA:
        case BARCODE_UPCE:
        case BARCODE_POSTNET:
        case BARCODE_MSI_PLESSEY:
        case BARCODE_FIM:
        case BARCODE_LOGMARS:
        case BARCODE_PHARMA:
        case BARCODE_PZN:
        case BARCODE_PHARMA_TWO:
        case BARCODE_PDF417:
        case BARCODE_PDF417TRUNC:
        case BARCODE_MAXICODE:
        case BARCODE_QRCODE:
        case BARCODE_CODE128B:
        case BARCODE_AUSPOST:
        case BARCODE_AUSREPLY:
        case BARCODE_AUSROUTE:
        case BARCODE_AUSREDIRECT:
        case BARCODE_ISBNX:
        case BARCODE_RM4SCC:
        case BARCODE_DATAMATRIX:
        case BARCODE_EAN14:
        case BARCODE_NVE18:
        case BARCODE_JAPANPOST:
        case BARCODE_KOREAPOST:
        case BARCODE_RSS14STACK:
        case BARCODE_RSS14STACK_OMNI:
        case BARCODE_RSS_EXPSTACK:
        case BARCODE_PLANET:
        case BARCODE_MICROPDF417:
        case BARCODE_ONECODE:
        case BARCODE_PLESSEY:
        case BARCODE_TELEPEN_NUM:
        case BARCODE_ITF14:
        case BARCODE_KIX:
        case BARCODE_AZTEC:
        case BARCODE_DAFT:
        case BARCODE_MICROQR:
        case BARCODE_HIBC_128:
        case BARCODE_HIBC_39:
        case BARCODE_HIBC_DM:
        case BARCODE_HIBC_QR:
        case BARCODE_HIBC_PDF:
        case BARCODE_HIBC_MICPDF:
        case BARCODE_HIBC_AZTEC:
        case BARCODE_AZRUNE:
        case BARCODE_CODE32:
        case BARCODE_EANX_CC:
        case BARCODE_EAN128_CC:
        case BARCODE_RSS14_CC:
        case BARCODE_RSS_LTD_CC:
        case BARCODE_RSS_EXP_CC:
        case BARCODE_UPCA_CC:
        case BARCODE_UPCE_CC:
        case BARCODE_RSS14STACK_CC:
        case BARCODE_RSS14_OMNI_CC:
        case BARCODE_RSS_EXPSTACK_CC:
        case BARCODE_CHANNEL:
        case BARCODE_CODEONE:
        case BARCODE_GRIDMATRIX:
            result = 1;
            break;
    }

    return result;
}

int extended_charset(struct zint_symbol *symbol, unsigned char *source, int length)
{
    int error_number = 0;

    /* These are the "elite" standards which can support multiple character sets */
    switch(symbol->symbology) {
        case BARCODE_QRCODE: error_number = qr_code(symbol, source, length); break;
    }

    return error_number;
}

int reduced_charset(struct zint_symbol *symbol, unsigned char *source, int length)
{
    /* These are the "norm" standards which only support Latin-1 at most */
    int error_number = 0;

#ifndef _MSC_VER
    unsigned char preprocessed[length + 1];
#else
        unsigned char* preprocessed = (unsigned char*)_alloca(length + 1);
#endif

    switch(symbol->input_mode) {
        case DATA_MODE:
        case UNICODE_MODE:
            error_number = latin1_process(symbol, source, preprocessed, &length);
            if(error_number != 0) { return error_number; }
            break;
    }

    switch(symbol->symbology) {
        case BARCODE_PDF417: error_number = pdf417enc(symbol, preprocessed, length); break;
        case BARCODE_AZTEC: error_number = aztec(symbol, preprocessed, length); break;
    }

    return error_number;
}

int ZBarcode_Encode(struct zint_symbol *symbol, unsigned char *source, int length)
{
    int error_number, error_buffer, i;
        error_number = 0;

    if(length == 0) {
        length = ustrlen(source);
    }
    if(length == 0) {
        strcpy(symbol->errtxt, "No input data");
        error_tag(symbol->errtxt, ERROR_INVALID_DATA);
        return ERROR_INVALID_DATA;
    }

    if(strcmp(symbol->outfile, "") == 0) {
        strcpy(symbol->outfile, "out.png");
    }
#ifndef _MSC_VER
        unsigned char local_source[length + 1];
#else
        unsigned char* local_source = (unsigned char*)_alloca(length + 1);
#endif

    /* First check the symbology field */
    if(symbol->symbology < 1) { strcpy(symbol->errtxt, "Symbology out of range, using Code 128"); symbol->symbology = BARCODE_CODE128; error_number = WARN_INVALID_OPTION; }

    if(error_number > 4) {
        error_tag(symbol->errtxt, error_number);
        return error_number;
    } else {
        error_buffer = error_number;
    }

    if((symbol->input_mode < 0) || (symbol->input_mode > 2)) { symbol->input_mode = DATA_MODE; }

    memcpy(local_source, source, length);
    local_source[length] = '\0';

    switch(symbol->symbology) {
        case BARCODE_QRCODE:
            error_number = extended_charset(symbol, local_source, length);
            break;
        default:
            error_number = reduced_charset(symbol, local_source, length);
            break;
    }

    if(error_number == 0) {
        error_number = error_buffer;
    }
    error_tag(symbol->errtxt, error_number);
    /*printf("%s\n",symbol->text);*/
    return error_number;
}

int ZBarcode_Buffer(struct zint_symbol *symbol, int rotate_angle)
{
    int error_number;

    switch(rotate_angle) {
        case 0:
        case 90:
        case 180:
        case 270:
            break;
        default:
            strcpy(symbol->errtxt, "Invalid rotation angle");
            return ERROR_INVALID_OPTION;
            break;
    }

    error_number = bmp_handle(symbol, rotate_angle);
    error_tag(symbol->errtxt, error_number);
    return error_number;
}

int ZBarcode_Encode_and_Buffer(struct zint_symbol *symbol, unsigned char *input, int length, int rotate_angle)
{
    int error_number;

    error_number = 0;

    error_number = ZBarcode_Encode(symbol, input, length);
    if(error_number != 0) {
        return error_number;
    }

    error_number = ZBarcode_Buffer(symbol, rotate_angle);
    return error_number;
}
