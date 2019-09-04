package com.simplex.coin2credit.ndef

import android.nfc.NdefRecord
import android.nfc.Tag
import android.nfc.tech.Ndef
import android.os.AsyncTask
import com.simplex.coin2credit.database.TransactionInterface
import java.nio.charset.Charset
import java.util.*
import kotlin.experimental.and

class NdefReader(context: TransactionInterface): AsyncTask<Tag, Void, String>() {
    private var mContext = context

    override fun doInBackground(vararg tag: Tag?): String {
        val records = Ndef.get(tag[0]).cachedNdefMessage.records

        for(record in records) {
            if(record.tnf == NdefRecord.TNF_WELL_KNOWN && Arrays.equals(record.type, NdefRecord.RTD_TEXT)) {
                return readText(record)
            }
        }

        return ""
    }

    override fun onPostExecute(result: String?) {
        requireNotNull(result)
        mContext.relayMessage(result)
    }

    private fun readText(record: NdefRecord): String = record.payload.let { payload ->
            val textEncoding = if (payload[0] and 128.toByte() == 0.toByte()) "UTF-8" else "UTF-16"
            val languageCodeLength = payload[0] and 63.toByte()

            return@let String(payload, languageCodeLength + 1,
                    payload.size - 1 - languageCodeLength, Charset.forName(textEncoding))
        }
}