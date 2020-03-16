//
// Created by Omar Shahbaz Khan on 13/03/2020.
//

#ifndef EXQUISITOR_EXQCONSTANTS_H
#define EXQUISITOR_EXQCONSTANTS_H

/* Modalities and modality activity flags */
#define N_MOD           2
#define VIS             0
#define TXT             1

#define VIS_ONLY        0
#define TXT_ONLY        1
#define MM              2

/* Data types of the Ratio-64 representation */
#define TOP_FEATURES_PATH   0
#define FEATURE_IDS_PATH    1
#define FEATURE_RATIOS_PATH 2

/* Maximum number of positives and negatives kept by the model */
#define N_MAX_POS       500
#define N_MAX_NEG       5000

/* Train data constants and flags */
#define N_TRAIN_TYPE    2
#define POS             0
#define NEG             1
#define CLASS_POS       1.0
#define CLASS_NEG       -1.0

/* SVM parameters (currently hard-coded) */
#define SVM_LAMBDA      0.01
#define SVM_EPOCHS      100

/* Ratio-64: initial feature (F) bitfield parameters */
//#define INIT_SHIFT      54
#define INIT_SHIFT      48
//#define INIT_MASK       ((unsigned long long)18014398509481983)
#define INIT_MASK       ((unsigned long long)281474976710655)
//#define INIT_N_VALS     ((double)10000000000000000.0)
#define INIT_N_VALS     ((double)200000000000000.0)

/* Ratio-64: feature ID and ratio (I, R) bitfield parameters */
//#define N_ENC_MASK      15
//#define FEAT_MASK       1023
#define FEAT_MASK       65535
//#define FEAT_DIVISOR    ((double)1000.0)
#define FEAT_DIVISOR    ((double)50000.0)
//#define N_FEAT_PER_INT  6
#define N_FEAT_PER_INT 4

/* Blackthorn eCP constants */
#define QUERY_X             5
#define QUERY_Y             2
#define N_TOP_FEAT          4
#define N_TOTAL_FEAT        12988
#define N_TOTAL_DESC_PER_B  1000000

#endif //EXQUISITOR_EXQCONSTANTS_H
