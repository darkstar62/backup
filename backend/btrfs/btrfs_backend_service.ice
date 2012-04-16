module backend {

struct BackupSetMessage {
  // Unique identifier of the backup set in the backend.
  long id;

  // Descriptive name of the backup set.
  string name;
};

sequence<BackupSetMessage> BackupSetList;

interface BtrfsBackendService {
  // Initiate a ping to verify the service is alive.
  void Ping();

  // Get all the backup sets managed by the backend.
  BackupSetList EnumerateBackupSets();
};

};  // module backend

