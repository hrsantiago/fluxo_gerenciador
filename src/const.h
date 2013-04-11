#ifndef CONST_H
#define CONST_H

#define EXIT_RESTART 2
#define DIALOG_MIN_WIDTH 480
#define BACKUP_TIME 60*1000

enum ContractState {
    STATE_IN_PROGRESS,
    STATE_COMPLETED,
    STATE_CANCELED
};

enum ProposalState {
    STATE_PENDING,
    STATE_SENT,
    STATE_ACCEPTED,
    STATE_DECLINED
};

#endif // CONST_H
