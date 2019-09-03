package com.simplex.coin2credit.database

import android.os.AsyncTask
import java.text.SimpleDateFormat
import java.util.*

class AsyncStore(private val database: AppDatabase): AsyncTask<String, Void, Void>() {

    override fun doInBackground(vararg params: String?): Void? {
        val items = ArrayList<String?>().apply {
            for (p in params) add(p)
        }

        val simpleDate = SimpleDateFormat("MM/dd/yyyy", Locale.US)
        val strDt = simpleDate.format(Calendar.getInstance().time)

        val t = Transaction().apply {
            date = strDt
            amount = items[0]
        }

        database.transactionDao().insertAll(t)

        return null
    }
}