package com.simplex.coin2credit

import android.nfc.NdefRecord
import android.nfc.Tag
import android.nfc.tech.Ndef
import android.os.AsyncTask
import java.nio.charset.Charset
import java.util.*
import kotlin.experimental.and

class NdefReader(context: TransactionInterface): AsyncTask<Tag, Void, String>() {
    private var mContext = context

    override fun doInBackground(vararg p0: Tag?): String {
        val tag = p0[0]
        val ndef = Ndef.get(tag)
        val ndefMessage = ndef.cachedNdefMessage
        val records = ndefMessage.records

        for(ndefRecord in records) {
            if(ndefRecord.tnf == NdefRecord.TNF_WELL_KNOWN &&
                    Arrays.equals(ndefRecord.type, NdefRecord.RTD_TEXT)) {
                try {
                    return readText(ndefRecord)
                } catch (e: Exception) {
                    e.printStackTrace()
                }
            }
        }

        return ""
    }

    private fun readText(record: NdefRecord): String {
        val payload = record.payload
        val textEncoding = if (payload[0] and 128.toByte() == 0.toByte()) "UTF-8" else "UTF-16"

        // Get the Language Code
        val languageCodeLength = payload[0] and 63.toByte()

        return String(payload, languageCodeLength + 1,
                payload.size - 1 - languageCodeLength, Charset.forName(textEncoding))

    }

    override fun onPostExecute(result: String?) {
        if(result != null) {
            mContext.relayMessage(result)
            //Toast.makeText(mContext, "Content: " + result, Toast.LENGTH_LONG).show()
        }
    }

}