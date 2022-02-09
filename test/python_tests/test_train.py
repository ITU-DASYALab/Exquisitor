import argparse;
import exq;


def single_modality_initialize():
    iota = 1
    noms = 100
    num_workers = 1
    segments = 16
    num_modalities = 1
    b = 256
    comp_conf_files = ['../data/lsc_100K/imagenet13k/img13k_100k_index_full.cnfg']
    mod_feature_dimensions = [12988]
    func_type = 0
    func_objs = [[5, 48, 16, 16, 281474976710655, float(2*pow(10,14)), 65535, 65535, 50000.0]]
    item_metadata = []
    video_metadata = []
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)


def two_modalities_initialize():
    return 0


def test_single_modality_no_filters():
    # 3 Positive images of indoor setting (1 with a clock). 1 Negative outdoor image
    item_ids = [39310,17230,73524,65850,54647]
    labels = [1.0,1.0,1.0,-1.0,-1.0]
    train_ret = exq.train(item_ids, labels)
    print(train_ret)
    return 0


def test_single_modality_filters_grc():
    return 0


def test_single_modality_filters_frc():
    return 0


def test_single_modality_filters_erc():
    return 0


def test_single_modality_filters_arc():
    return 0


def test_two_modalities_no_filters():
    return 0


def test_two_modalities_filters_grc():
    return 0


def test_two_modalities_filters_frc():
    return 0


def test_two_modalities_filters_erc():
    return 0


def test_two_modalities_filters_arc():
    return 0


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Test Exquisitor's initialize function")
    parser.add_argument('test_group', type=int, help='')
    args = parser.parse_args()

    if args.test_group == 0:
        single_modality_initialize()
        test_single_modality_no_filters()
        exit()
    elif args.test_group == 1:
        exit()
    elif args.test_group == 2:
        exit()
    elif args.test_group == 3:
        exit()
    elif args.test_group == 4:
        exit()
