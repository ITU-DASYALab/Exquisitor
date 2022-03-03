import argparse
import exq
import datetime
from time import time
import json
import csv


# Filter parameters
ids_file = '../data/lsc_100K/meta21/lsc21.txt'
meta_file = '../data/lsc_100K/meta21/lsc2020-metadata.csv'
mediainfo_file = '../data/lsc_100K/meta21/mediainfo21.json'
locations_file = open('../data/lsc_100K/meta21/distinctLocations.txt', 'r')
activities_file = open('../data/lsc_100K/meta21/distinctActivities.txt', 'r')
day_arr = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday']
year_arr = [2015, 2016, 2018]

# LSC BAD IMAGE NAME FIXES
badDates = {}
badDates['20160921'] = {}
badDates['20160921']['rr.jpg'] = '100000'
badDates['20160921']['rr1.jpg'] = '120000'
badDates['20160922'] = {}
badDates['20160922']['rr1.jpg'] = '100000'
badDates['20160923'] = {}
badDates['20160923']['rr.jpg'] = '110000'
badDates['20161003'] = {}
badDates['20161003']['g.jpg'] = '180000'
badDates['20161003']['rr.jpg'] = '110000'
badDates['20161003']['rr1.jpg'] = '160000'
badDates['20161004'] = {}
badDates['20161004']['rr.jpg'] = '090000'

ids_map = {}
ids_reverse_meta_map = dict()
locations = map(str.strip, locations_file.readlines())
locations_dict = dict()
for idx, s in enumerate(locations):
    locations_dict[s] = idx
locations_file.close()

activities = map(str.strip, activities_file.readlines())
activities_dict = dict()
for idx, s in enumerate(activities):
    activities_dict[s] = idx
activities_file.close()

mediainfo = {}
with open(mediainfo_file, 'r') as f:
    mediainfo = json.load(f)

imgVidMap = {}
for key in mediainfo:
    for arr in mediainfo[key]['shots']:
        imgVidMap[arr['exqId']] = key

dayIndexMap = {}
for idx, key in enumerate(sorted(mediainfo.keys())):
    dayIndexMap[key] = idx


def initialize_metadata():
    with open(ids_file) as f:
        for idx, s in enumerate(f.readlines()[0:100000]):
            ids_map[idx] = dict()
            ids_map[idx]['name'] = s.strip()
            split = ids_map[idx]['name'].split('_')
            dt = ''
            t = ''
            if len(split) == 1:
                ids_map[idx]['name'] = ids_map[idx-4]['name'].split('_')[0] + '_' + split[0]
                dt = ids_map[idx-4]['name'].split('_')[0]
                t = badDates[dt][split[0] + '.jpg']
            elif len(split) == 4:
                dt = split[2]
                t = split[3]
            elif len(split) == 3:
                dt = split[0]
                t = split[1]
            else:
                print(s, split)
            yy = dt[:4]
            mm = dt[4:6]
            dd = dt[6:8]
            date = "%s/%s/%s" % (yy, mm, dd)
            day = datetime.date(int(yy), int(mm), int(dd)).weekday()
            ids_map[idx]['locationId'] = 167
            ids_map[idx]['location'] = 'unknown'
            ids_map[idx]['activityId'] = 3
            ids_map[idx]['activity'] = 'unknown'
            ids_map[idx]['hour'] = int(t[0:2])
            ids_map[idx]['day'] = day
            ids_map[idx]['year'] = year_arr.index(int(yy))
            ids_reverse_meta_map[(dt + '_' + t[0:4])] = idx

    with open(meta_file) as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        for row in csv_reader:
            img_id = row[0]
            loc = str.strip(row[6]).lower()
            act = str.strip(row[11]).lower()
            if img_id in ids_reverse_meta_map:
                if loc == '' or loc == 'null':
                    loc = 'unknown'
                if act == '' or act == 'null':
                    act = 'unknown'
                ids_map[ids_reverse_meta_map[img_id]]['locationId'] = locations_dict.get(loc, 167)
                ids_map[ids_reverse_meta_map[img_id]]['location'] = loc
                ids_map[ids_reverse_meta_map[img_id]]['activityId'] = activities_dict.get(act, 3)
                ids_map[ids_reverse_meta_map[img_id]]['activity'] = act


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


def single_modality_initialize_with_metadata():
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
    n_items = len(ids_map)
    items_meta = []
    for i in range(n_items):
        item = [0, False, 0, [], [
            [ids_map[i]['locationId']],
            [ids_map[i]['activityId']],
            [ids_map[i]['hour']],
            [ids_map[i]['day']],
            [ids_map[i]['year']],
            [dayIndexMap[imgVidMap[i]]]
        ]]
        items_meta.append(item)
    item_metadata = items_meta
    video_metadata = []
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)


def two_modalities_initialize():
    iota = 1
    noms = 1000
    num_workers = 1
    segments = 16
    num_modalities = 2
    b = 256
    comp_conf_files = ['../data/yfcc/vis_index_full.cnfg', '../data/yfcc/txt_index_full.cnfg']
    mod_feature_dimensions = [1000, 100]
    func_type = 1
    func_objs = [[7, 54, 10, 10, 18014398509481983, float(pow(10, 16)), 1023, 1023, 1000.0]]
    item_metadata = []
    video_metadata = []
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)
    return 0


def three_modalities_initialize():
    iota = 1
    noms = 1000
    num_workers = 1
    segments = 16
    num_modalities = 3
    b = 256
    comp_conf_files = ['../data/vbs/index/plain/imgnet_index_full.cnfg',
                       '../data/vbs/index/plain/actions_mid_index_full.cnfg',
                       '../data/vbs/index/plain/places_index_full.cnfg']
    mod_feature_dimensions = [12988, 700, 365]
    func_type = 0
    func_objs = [
        [5, 48, 16, 16, pow(2, 32)-1, float(pow(2, 32)), pow(2, 16)-1, pow(2, 16)-1, pow(2, 16)],
        [7, 54, 10, 10, pow(2, 32)-1, float(pow(2, 32)), pow(2, 10)-1, pow(2, 10)-1, pow(2, 10)],
        [8, 55, 9, 9, pow(2, 32)-1, float(pow(2, 32)), pow(2, 9)-1, pow(2, 9)-1, pow(2, 9)]
    ]
    item_metadata = []
    video_metadata = []
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)

    return 0


def test_single_modality_no_filters():
    # 3 Positive images of indoor setting (1 with a clock). 1 Negative outdoor image
    item_ids = [39310, 17230, 73524, 65850, 54647]
    labels = [1.0, 1.0, 1.0, -1.0, -1.0]
    train_ret = exq.train(item_ids, labels, False, [])
    print(train_ret)
    return 0


def test_single_modality_no_filters_no_training_items():
    # 3 Positive images of indoor setting (1 with a clock). 1 Negative outdoor image
    item_ids = []
    labels = []
    train_ret = exq.train(item_ids, labels, False, [])
    print(train_ret)
    return 0


def test_single_modality_filters_grc():
    # 3 Positive images of indoor setting (1 with a clock). 1 Negative outdoor image
    item_ids = [39310, 17230, 73524, 65850, 54647]
    labels = [1.0, 1.0, 1.0, -1.0, -1.0]
    collections = []
    std_filters = []
    coll_filters = [
        [
            [46],  # location (home)
            [],  # activity
            [12],  # hour
            [4],  # day (Friday)
            []  # year
        ]
    ]
    vid_filters = []
    filters = [collections, std_filters, coll_filters, vid_filters]
    train_ret = exq.train(item_ids, labels, True, filters)
    print(train_ret)
    return 0


def test_single_modality_filters_frc():
    return 0


def test_single_modality_filters_erc():
    return 0


def test_single_modality_filters_arc():
    return 0


def test_two_modalities_no_filters():
    # Arbitrary items from the collections, meant to test if the training phase works
    item_ids = [39310, 17230, 73524, 65850, 54647]
    labels = [1.0, 1.0, 1.0, -1.0, -1.0]
    train_ret = exq.train(item_ids, labels, False, [])
    print(train_ret)
    return 0


def test_two_modalities_filters_grc():
    return 0


def test_two_modalities_filters_frc():
    return 0


def test_two_modalities_filters_erc():
    return 0


def test_two_modalities_filters_arc():
    return 0


def test_three_modalities_no_filters():
    # Arbitrary items from the collections, meant to test if the training phase works
    item_ids = [39310, 17230, 73524, 65850, 54647]
    labels = [1.0, 1.0, 1.0, -1.0, -1.0]
    train_ret = exq.train(item_ids, labels, False, [])
    print(train_ret)
    return 0


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Test Exquisitor's initialize function")
    parser.add_argument('test_group', type=int, help='')
    args = parser.parse_args()

    if args.test_group == 0:
        single_modality_initialize()
        test_single_modality_no_filters()
        test_single_modality_no_filters_no_training_items()
        exq.reset_model()
        exit()
    elif args.test_group == 1:
        initialize_metadata()
        single_modality_initialize_with_metadata()
        test_single_modality_filters_grc()
        # test_single_modality_filters_frc()
        # test_single_modality_filters_erc()
        # test_single_modality_filters_arc()
        exq.reset_model()
        exit()
    elif args.test_group == 2:
        two_modalities_initialize()
        test_two_modalities_no_filters()
        exit()
    elif args.test_group == 3:
        three_modalities_initialize()
        test_two_modalities_no_filters()
        exit()
    elif args.test_group == 4:
        exit()
    # exq.safe_close() # Not implemented yet
