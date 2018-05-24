package com.simplex.coin2credit

import android.os.AsyncTask
import java.text.SimpleDateFormat
import java.util.*

class AsyncStore(database: AppDatabase): AsyncTask<String, Void, Void>() {
    private val DB = database

    override fun doInBackground(vararg params: String?): Void? {
        // ...
        var items = ArrayList<String?>()
        for(p in params) {
            items.add(p)
        }
        val cal = Calendar.getInstance()
        val simpleDate = SimpleDateFormat("MM/dd/yyyy", Locale.US)
        val strDt = simpleDate.format(cal.time)

        val t = Transaction()
        t.date = strDt
        t.amount = items[0]

        DB.transactionDao().insertAll(t)

        return null
    }
}