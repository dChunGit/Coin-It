package com.simplex.coin2credit.database

import androidx.room.Database
import androidx.room.Room
import androidx.room.RoomDatabase
import android.content.Context

import com.commonsware.cwac.saferoom.SafeHelperFactory

@Database(entities = [Transaction::class], version = 1, exportSchema = false)
abstract class AppDatabase : RoomDatabase() {
    abstract fun transactionDao(): TransactionDao

    companion object {
        private var INSTANCE: AppDatabase? = null

        fun getAppDatabase(context: Context, password: CharArray): AppDatabase = INSTANCE ?: {
            val db = Room.databaseBuilder(context.applicationContext, AppDatabase::class.java, "transaction_database")
                    .openHelperFactory(SafeHelperFactory(password))
                    .build()

            INSTANCE = db
            db
        }()

        fun destroyDatabase() {
            INSTANCE = null
        }
    }
}
