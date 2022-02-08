import argparse;
import exq;


def test_one_modality():
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
    return 0


def test_two_modalities():
    iota = 1
    noms = 100
    num_workers = 1
    segments = 16
    num_modalities = 1
    b = 256
    comp_conf_files = [[]]
    mod_feature_dimensions = []
    func_type = 0
    func_objs = [[]]
    item_metadata = [[]]
    video_metadata = [[]]
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)
    return 0


def test_two_modalities_different_compression():
    iota = 1
    noms = 100
    num_workers = 1
    segments = 16
    num_modalities = 1
    b = 256
    comp_conf_files = [[]]
    mod_feature_dimensions = []
    func_type = 0
    func_objs = [[]]
    item_metadata = [[]]
    video_metadata = [[]]
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)
    return 0


def test_three_modalities():
    iota = 1
    noms = 100
    num_workers = 1
    segments = 16
    num_modalities = 1
    b = 256
    comp_conf_files = [[]]
    mod_feature_dimensions = []
    func_type = 0
    func_objs = [[]]
    item_metadata = [[]]
    video_metadata = [[]]
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)
    return 0


def test_three_modalities_different_compression():
    iota = 1
    noms = 100
    num_workers = 1
    segments = 16
    num_modalities = 1
    b = 256
    comp_conf_files = [[]]
    mod_feature_dimensions = []
    func_type = 0
    func_objs = [[]]
    item_metadata = [[]]
    video_metadata = [[]]
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)

    return 0


def test_filters_one_collection():
    iota = 1
    noms = 100
    num_workers = 1
    segments = 16
    num_modalities = 1
    b = 256
    comp_conf_files = [[]]
    mod_feature_dimensions = []
    func_type = 0
    func_objs = [[]]
    item_metadata = [[]]
    video_metadata = [[]]
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)
    return 0


def test_filters_two_collections():
    iota = 1
    noms = 100
    num_workers = 1
    segments = 16
    num_modalities = 1
    b = 256
    comp_conf_files = [[]]
    mod_feature_dimensions = []
    func_type = 0
    func_objs = [[]]
    item_metadata = [[]]
    video_metadata = [[]]
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)
    return 0


def test_filters_videos_one_collection():
    iota = 1
    noms = 100
    num_workers = 1
    segments = 16
    num_modalities = 1
    b = 256
    comp_conf_files = [[]]
    mod_feature_dimensions = []
    func_type = 0
    func_objs = [[]]
    item_metadata = [[]]
    video_metadata = [[]]
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)
    return 0


def test_filters_videos_two_collections():
    iota = 1
    noms = 100
    num_workers = 1
    segments = 16
    num_modalities = 1
    b = 256
    comp_conf_files = [[]]
    mod_feature_dimensions = []
    func_type = 0
    func_objs = [[]]
    item_metadata = [[]]
    video_metadata = [[]]
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)
    return 0


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Test Exquisitor's initialize function")
    parser.add_argument('test_group', type=int, help='')
    args = parser.parse_args()

    if args.test_group == 0:
        test_one_modality()
    elif args.test_group == 1:
        test_two_modalities()
        test_three_modalities()
    elif args.test_group == 2:
        test_two_modalities_different_compression()
        test_three_modalities_different_compression()
    elif args.test_group == 3:
        test_filters_one_collection()
        test_filters_two_collections()
    elif args.test_group == 4:
        test_filters_videos_one_collection()
        test_filters_videos_two_collections()
