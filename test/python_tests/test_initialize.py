import argparse;
import exq;


def test_one_modality():
    iota = 1
    noms = 100
    num_workers = 1
    segments = 16
    num_modalities = 1
    b = 256
    comp_conf_files = [[]]
    mod_feature_dimensions = [12988]
    func_type = 0
    func_objs = [[]]
    item_metadata = [[]]
    video_metadata = [[]]
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
    parser.add_argument('par', type=int, help='')
    parser.add_argument('par', type=int, help='')

    test_one_modality()

    test_two_modalities()
    test_three_modalities()

    test_two_modalities_different_compression()
    test_three_modalities_different_compression()

    test_filters_one_collection()
    test_filters_two_collections()

    test_filters_videos_one_collection()
    test_filters_videos_two_collections()
