// ----------------------------------------------------
// RAWTEX
// Gimp 2.x plugin to create raw textures & pictures.
//
// mailto:charlet.franck@wanadoo.fr
// http://franck.charlet.pagesperso-orange.fr/
//
// Copyright (C) 2006-2022 Franck Charlet.
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------

// ----------------------------------------------------
// Includes
#include "rawtex.h"

// ----------------------------------------------------
// Fucntions
extern const gchar *(*gimp_plug_in_directory_)(void);

// ----------------------------------------------------
// Variables
char *Config_Keys[] =
{
    "Dest_Format",
    "Dest_Palette_Format",
    "Nbr_MipMaps",
    "Use_Alpha_Color",
    "Alpha_Color",
    "Swizzle_Texture",
    "Use_Swizzle_Texture",
    "Save_Picture_Source",
    "Save_Palette_Source",
    "Dxt_Type",
    "Use_BGR",
    "Idx_Type",
};
char Config_FileName[512];

// ----------------------------------------------------
// Check the validity of a key
int Get_Config_Key(char *KeyName)
{
    int i;

    for(i = 0; i < sizeof(Config_Keys) / 4; i++)
    {
        if(!strcmp(KeyName, Config_Keys[i])) return(i);
    }
    return(-1);
}

// ----------------------------------------------------
// Fill the default values structure from the config file
void Load_Config(LPPluginValues Values)
{
    FILE *Config_File;
    char Config_Key[64];
    int Config_Value;
    int i;
    int Config_Idx;
    int phony;
#ifdef G_OS_WIN32
    sprintf(Config_FileName, "%s\\rawtex.config", gimp_plug_in_directory_());
#else
    sprintf(Config_FileName, "%s\\rawtex.config", gimp_plug_in_directory());
#endif
    Config_File = fopen(Config_FileName, "r");
    if(Config_File)
    {
        phony = fscanf(Config_File, "%s", (char *) &Config_Key);
        if(!strcmp(Config_Key, "RAWTEX"))
        {
            for(i = 0; i < sizeof(Config_Keys) / 4; i++)
            {
                phony = fscanf(Config_File, "%s %d", (char *) &Config_Key, &Config_Value);
                Config_Idx = Get_Config_Key(Config_Key);
                if(Config_Idx == -1) break;
                switch(Config_Idx)
                {
                    case 0:
                        Values->Dest_Format = Config_Value;
                        break;
                    case 1:
                        Values->Dest_Palette_Format = Config_Value;
                        break;
                    case 2:
                        Values->Nbr_MipMaps = Config_Value;
                        break;
                    case 3:
                        Values->Use_Alpha_Color = Config_Value;
                        break;
                    case 4:
                        Values->Alpha_Color = Config_Value;
                        break;
                    case 5:
                        Values->Swizzle_Texture = Config_Value;
                        break;
                    case 6:
                        Values->Use_Swizzle_Texture = Config_Value;
                        break;
                    case 7:
                        Values->Save_Picture_Source = Config_Value;
                        break;
                    case 8:
                        Values->Save_Palette_Source = Config_Value;
                        break;
                    case 9:
                        Values->Dxt_Type = Config_Value;
                        break;
                    case 10:
                        Values->Use_BGR = Config_Value;
                        break;
                    case 11:
                        Values->Idx_Type = Config_Value;
                        break;
                }
            }
            switch(Values->Dest_Format)
            {
                case GU_PSM_DXT1:
                case GU_PSM_DXT3:
                case GU_PSM_DXT5:
                    Values->Use_Swizzle_Texture = FALSE;
                    break;
            }
        }
        fclose(Config_File);
    }
}

// ----------------------------------------------------
// Save the config file
void Save_Config(LPPluginValues Values)
{
    FILE *Config_File;
#ifdef G_OS_WIN32
    sprintf(Config_FileName, "%s\\rawtex.config", gimp_plug_in_directory_());
#else
    sprintf(Config_FileName, "%s\\rawtex.config", gimp_plug_in_directory());
#endif
    Config_File = fopen(Config_FileName, "w");
    if(Config_File)
    {
        fprintf(Config_File, "RAWTEX\n");
        fprintf(Config_File, "%s %d\n", Config_Keys[0], Values->Dest_Format);
        fprintf(Config_File, "%s %d\n", Config_Keys[1], Values->Dest_Palette_Format);
        fprintf(Config_File, "%s %d\n", Config_Keys[2], Values->Nbr_MipMaps);
        fprintf(Config_File, "%s %d\n", Config_Keys[3], Values->Use_Alpha_Color);
        fprintf(Config_File, "%s %d\n", Config_Keys[4], Values->Alpha_Color);
        fprintf(Config_File, "%s %d\n", Config_Keys[5], Values->Swizzle_Texture);
        fprintf(Config_File, "%s %d\n", Config_Keys[6], Values->Use_Swizzle_Texture);
        fprintf(Config_File, "%s %d\n", Config_Keys[7], Values->Save_Picture_Source);
        fprintf(Config_File, "%s %d\n", Config_Keys[8], Values->Save_Palette_Source);
        fprintf(Config_File, "%s %d\n", Config_Keys[9], Values->Dxt_Type);
        fprintf(Config_File, "%s %d\n", Config_Keys[10], Values->Use_BGR);
        fprintf(Config_File, "%s %d\n", Config_Keys[11], Values->Idx_Type);
        fclose(Config_File);
    }
}
