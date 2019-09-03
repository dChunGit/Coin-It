package com.simplex.coin2credit.database

import android.os.AsyncTask

class AsyncGet(private val database: AppDatabase, private val tint: TransactionInterface): AsyncTask<Void, Void, Void>() {
    private lateinit var values: List<Transaction>

    override fun doInBackground(vararg params: Void?): Void? {
        values = database.transactionDao().all
        return null
    }

    override fun onPostExecute(result: Void?) {
        super.onPostExecute(result)
        tint.databaseRead(values)
    }
}