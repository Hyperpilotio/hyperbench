#!/usr/bin/env python
"""
Collects benchmark configurations in benchmarks.json and save them into MongoDB
"""

import sys
import json
from pymongo import MongoClient

MONGO_URL = "internal-mongo-elb-624130134.us-east-1.elb.amazonaws.com:27017"
MONGO_USER = "analyzer"
MONGO_PASSWORD = "hyperpilot"
CONFIGDB_NAME = "configdb"

if __name__ == "__main__":
    if len(sys.argv) > 1:
        MONGO_URL = sys.argv[1]

    configdb = MongoClient(MONGO_URL).get_database(CONFIGDB_NAME)
    configdb.authenticate(MONGO_USER, MONGO_PASSWORD)

    with open("benchmarks.json") as f:
        benchmarks = json.load(f)
        for config in benchmarks["benchmarks"]:
            print "Writing benchmark " + config["name"]
            configdb.benchmarks.replace_one(
                filter={"name": config["name"]},
                replacement=config,
                upsert=True
            )
