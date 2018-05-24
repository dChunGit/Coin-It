package com.simplex.coin2credit

import android.os.AsyncTask
import java.text.SimpleDateFormat
import java.util.*

class AsyncGet(database: AppDatabase, tint: TransactionInterface): AsyncTask<Void, Void, Void>() {
    private val DB = database
    private val transactionInt = tint
    private lateinit var values: List<Transaction>

    override fun doInBackground(vararg params: Void?): Void? {
        // ...
        values = DB.transactionDao().all

        return null
    }

    override fun onPostExecute(result: Void?) {
        super.onPostExecute(result)
        transactionInt.databaseRead(values)
    }
}