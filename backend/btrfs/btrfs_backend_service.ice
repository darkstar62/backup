module backup_proto {

struct BackupSetMessage {
  // Unique identifier of the backup set in the backend.  Only positive
  // integers are valid.
  long id;

  // Descriptive name of the backup set.
  string name;
};

sequence<BackupSetMessage> BackupSetList;

struct BackupDescriptor {
  // List of all backup sets managed by the backend.
  BackupSetList backup_sets;

  // The next ID to assign to a new backup set.
  long next_id;
};

interface BtrfsBackendService {
  // Initiate a ping to verify the service is alive.
  void Ping();

  // Get all the backup sets managed by the backend.
  BackupSetList EnumerateBackupSets();

  // Create a new backup set with the given name.  If successful, the passed
  // BackupSetMessage is filled with the details of the set and the function
  // returns true.  Otherwise, the passed message is left alone and the function
  // returns false.
  bool CreateBackupSet(string name, out BackupSetMessage set);
};

};  // module backend

