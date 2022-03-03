import argparse
import exq
import datetime
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
    func_objs = [[5, 48, 16, 16, 281474976710655, float(2*pow(10, 14)), 65535, 65535, 50000.0]]
    item_metadata = []
    video_metadata = []
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)
    return 0


def test_two_modalities():
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
    comp_conf_files = []
    mod_feature_dimensions = []
    func_type = 0
    func_objs = []
    item_metadata = []
    video_metadata = []
    exq.initialize(iota, noms, num_workers, segments, num_modalities, b, comp_conf_files, mod_feature_dimensions,
                   func_type, func_objs, item_metadata, video_metadata)
    return 0


def test_three_modalities_different_compression():
    iota = 1
    noms = 1000
    num_workers = 1
    segments = 16
    num_modalities = 1
    b = 256
    comp_conf_files = ['../data/vbs/index/plain/imgnet_index_full.cnfg',
                       '../data/vbs/index/plain/actions_mid_index_full.cnfg',
                       '../data/vbs/index/plain/places_index_full.cnfg']
    mod_feature_dimensions = [12988, 700, 365]
    func_type = 1
    func_objs = [
        [5, 48, 16, 16, pow(2, 32)-1, float(pow(2, 32)), pow(2, 16)-1, pow(2, 16)-1, pow(2, 16)],
        [7, 54, 10, 10, pow(2, 32)-1, float(pow(2, 32)), pow(2, 16)-1, pow(2, 16)-1, pow(2, 16)],
        [8, 55, 9, 9, pow(2, 32)-1, float(pow(2, 32)), pow(2, 9)-1, pow(2, 9)-1, pow(2, 9)]
    ]
    item_metadata = []
    video_metadata = []
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
    comp_conf_files = ['../data/lsc_100K/imagenet13k/img13k_100k_index_full.cnfg']
    mod_feature_dimensions = [12988]
    func_type = 0
    func_objs = [[5, 48, 16, 16, 281474976710655, float(2*pow(10, 14)), 65535, 65535, 50000.0]]
    n_items = len(ids_map)
    items_meta = []
    for i in range(n_items):
        item = []
        # collection id
        # with this properties when applying filters they will be sent as ([], [])
        # the first list holds the std filters where element 0 = collection id and 1 = video id (-1 = N/A)
        # followed by the applied standard filters
        # the second list holds the collection specific filters where for each collection there is a list
        # Example:
        # [collection_id: -1, video_id: -1, std_filters: []]
        # [[['home'],['walking'],[],['Monday','Thursday'],[],[]]]
        item.append(0)
        # is video?
        item.append(False)
        # video id
        item.append(0)
        # standard properties
        item.append([])
        # collection properties
        item.append([
            [ids_map[i]['locationId']],
            [ids_map[i]['activityId']],
            [ids_map[i]['hour']],
            [ids_map[i]['day']],
            [ids_map[i]['year']],
            [dayIndexMap[imgVidMap[i]]]
        ])
        items_meta.append(item)
    item_metadata = items_meta
    print(items_meta[0])
    print(items_meta[1])
    print(items_meta[2])
    video_metadata = []
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
        # test_three_modalities()
    elif args.test_group == 2:
        # test_two_modalities_different_compression()
        test_three_modalities_different_compression()
    elif args.test_group == 3:
        initialize_metadata()
        test_filters_one_collection()
        # test_filters_two_collections()
    elif args.test_group == 4:
        test_filters_videos_one_collection()
        test_filters_videos_two_collections()
