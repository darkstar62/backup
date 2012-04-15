module backend {

struct BackupSetMessage {
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

