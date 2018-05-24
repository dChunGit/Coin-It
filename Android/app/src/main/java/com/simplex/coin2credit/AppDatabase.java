package com.simplex.coin2credit;

import android.arch.persistence.room.Database;
import android.arch.persistence.room.Room;
import android.arch.persistence.room.RoomDatabase;
import android.content.Context;

import com.commonsware.cwac.saferoom.SafeHelperFactory;

@Database(entities = {Transaction.class}, version = 1, exportSchema = false)
public abstract class AppDatabase extends RoomDatabase{
    private static AppDatabase INSTANCE;
    public abstract TransactionDao transactionDao();

    public static AppDatabase getAppDatabase(Context context, char[] password) {
        if(INSTANCE == null) {
            SafeHelperFactory factory= new SafeHelperFactory(password);

            INSTANCE = Room.databaseBuilder(context.getApplicationContext(), AppDatabase.class, "transaction_database")
                    .openHelperFactory(factory)
                    .build();
        }

        return INSTANCE;
    }

    public static void destroyDatabase() {
        INSTANCE = null;
    }
}
