/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012  Simone Faro and Thierry Lecroq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 * 
 * contact the authors at: faro@dmi.unict.it, thierry.lecroq@univ-rouen.fr
 * download the tool at: http://www.dmi.unict.it/~faro/smart/
 */


int *PATT_SIZE;
int PATT_CUSTOM_SIZE[NumPatt];
int PATT_VERY_SHORT[NumPatt] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,0};
int PATT_SHORT_SIZE[NumPatt] = {2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,0};
int PATT_LARGE_SIZE[NumPatt] = {2,4,8,16,32,64,128,256,512,1024,2048,4096,0,0,0,0,0};
char *SETTING_BUFFER[NumSetting] = {"rand2", "rand4", "rand8", "rand16", "rand32", "rand64", "rand128", "rand250", "italianTexts", "englishTexts", "frenchTexts", "chineseTexts", "midimusic", "genome", "protein"};
int  SETTING_ALPHA_SIZE[NumSetting]= {2,4,8,16,32,64,128,250,128,128,128,128,128,64,64};

int EXECUTE[NumAlgo];
char *ALGO_NAME[NumAlgo];
char *ALGO_DESCRIPTION[NumAlgo];

#include "function.h"

enum algo_pos {
//Comparison based Algorithms
_BF, 			//Brute Force
_MP, 			//Morris-Pratt
_KMP, 			//Knuth-Morris Pratt
_BM, 			//Boyer-Moore
_HOR, 			//Hoorspool
_GS, 			//Galil Seiferas
_AG, 			//Apostolico-Giancarlo
_KR, 			//Karp-Rabin
_ZT, 			//Zhu-Takaoka
_AC, 			//Apostolico-Crochemore
_TW, 			// Two-Way
_OM, 			//Optimal Mismatch
_MS, 			//Maximal shift
_QS, 			//Quick-Search
_TunBM, 		//Tuned Boyer-Moore
_NSN, 			//not so naive
_TBM,			//Turbo Boyer-Moore
_Col, 			//Colussi
_Smith, 		//Smith
_GG, 			//Galil-Giancarlo
_Raita, 		//Raita
_SMOA, 			//String Mtahcing on Ordered Alphabets
_RCol, 			//Reverse Colussi
_Skip, 			//Skip Serach
_KMPSkip, 		//KMP skip search
_ASkip, 		//Alpha-Skip-Search
_BR, 			//Berry-ravindran
_AKC,          //AKC algorithm
_FS,           //Fast-Search                                      
_FFS,          //Forward-Fast-Search                              
_BFS,          //Backward-Fast-Search, Fast-Boyer-Moore           
_TS,           //Tailed-Substring                                 
_SSABS,        //Sheik-Sumit-Anindya-Balakrishnan-Sekar           
_TVSBS,        //Thathoo-Virmani-Sai-Balakrishnan-Sekar           
_PBMH,         //Boyer-Moore-Horspool using Probabilities         
_FJS,          //Franek-Jennings-Smyth                            
_BLOCK,        //2-Block Boyer-Moore                              
_HASH3,        //Wu-Manber for Single Pattern Matching (q=3)            
_HASH5,        //Wu-Manber for Single Pattern Matching (q=5)
_HASH8,        //Wu-Manber for Single Pattern Matching (q=8)            
_TSW,          //Two-Sliding-Window                               
_BMH2,         //Boyer-Moore-Horspool with q-grams  (BMH2)
_BMH4,         //Boyer-Moore-Horspool with q-grams  (BMH4)              
_GRASPm,       //Genomic Rapid Algorithm for String Pattern-match 
_SSEF,         //SSEF (K=7)  
//Algorithms based on automata
_AUT, 			//Automaton Matcher
_RF, 			//Revrese-Factor
_TRF, 			//Turbo Reverse factor
_Simon, 		//Simon 
_FDM, 			//Forward-DAWG-Match
_BOM, 			//BOM
_BOM2, 			//BOM2
_DFDM,         //Double Forward DAWG Matching                     
_WW,           //Wide Window                                      
_LDM,		   //Linear DAWG Matching                
_ILDM1,        //ILDM1
_ILDM2,        //ILDM2
_EBOM,         //Extended Backward Oracle Matching                
_FBOM,         //Forward Backward Oracle Matching                 
_SEBOM,        //Simplified Extended Backward Oracle Matching                
_SFBOM,        //Simplified Forward Backward Oracle Matching 
_SBDM,         //Succint Backward DAWG Matching                   
//Algorithms based on bit-parallelism
_SO, 			//Shift-Or
_SA, 			//Shift-And
_BNDM,         //BNDM for Long patterns                           
_BNDML,        //BNDM for Long patterns                           
_SBNDM,        //Simplified BNDM                                  
_TNDM,         //Two-Way Nondeterministic DAWG Matching           
_TNDMa,        //Two-Way Nondeterministic DAWG Matching (version 2)
_LBNDM,        //Long patterns BNDM                               
_SVM0,         //Shift Vector Matching (version 0)                                                       
_SVM1,         //Shift Vector Matching (version 1)                           
_SVM2,         //Shift Vector Matching (version 2)                                                       
_SVM3,         //Shift Vector Matching (version 3)                                                       
_SVM4,         //Shift Vector Matching (version 4)                                                       
_SBNDM2,       //Simplified BNDM with loop-unrolling              
_SBNDMBMH,     //SBNDM with Horspool Shift                         
_BMHSBNDM,     //Horspool with SBNDM test                          
_FNDM,         //Forward Nondeterministic DAWG Matching           
_BWW,          //Bit parallel Wide Window                         
_FAOSOq2,      //Fast Average Optimal Shift-Or (u=2)                   
_FAOSOq4,      //Fast Average Optimal Shift-Or (u=2)                   
_FAOSOq6,      //Fast Average Optimal Shift-Or (u=2)                   
_AOSO2,        //Average Optimal Shift-Or (q=2)                        
_AOSO4,        //Average Optimal Shift-Or (q=4)                        
_AOSO6,        //Average Optimal Shift-Or (q=6)                        
_BLIM,         //Bit-Parallel Length Invariant Matcher            
_FSBNDM,       //Forward SBNDM                                    
_BNDMq2,       //BNDM with q-grams                                
_BNDMq4,       //BNDM with q-grams                                
_BNDMq6,       //BNDM with q-grams                                
_SBNDMq2,      //Simplified BNDM with q-grams                     
_SBNDMq4,      //Simplified BNDM with q-grams                     
_SBNDMq6,      //Simplified BNDM with q-grams                     
_SBNDMq8,      //Simplified BNDM with q-grams                     
_UFNDMq2,      //Shift-Or with q-grams                            
_UFNDMq4,      //Shift-Or with q-grams                            
_UFNDMq6,      //Shift-Or with q-grams                            
_UFNDMq8,      //Shift-Or with q-grams                            
_SABP,		   //Small Alphabet Bit Parallel                                            
_DBWW,		   //DOUBLE BWW
_DBWW2,		   //DOUBLE BWW
_KSA,		   //Factorized SHIFT-AND                                            
_KBNDM,		   //Factorized BNDM

} ALGO_POS;

