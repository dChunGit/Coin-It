package com.simplex.coin2credit.ndef

import android.nfc.NdefMessage
import android.nfc.NdefRecord
import android.nfc.Tag
import android.nfc.tech.Ndef
import android.nfc.tech.NdefFormatable
import android.os.AsyncTask
import android.util.Log
import com.simplex.coin2credit.database.TransactionInterface
import java.io.ByteArrayOutputStream
import java.io.IOException
import java.nio.charset.Charset
import java.util.*

class NdefWriter(context: TransactionInterface, data: String): AsyncTask<Tag, Void, Boolean>() {
    private var mContext = context
    private var mData = data

    override fun doInBackground(vararg p0: Tag?): Boolean {
        Log.d(NDEF_TAG, "Writing $mData")
        val tag = p0[0] as Tag
        val content = mData.toByteArray(Charset.forName("UTF-8"))
        val lang = Locale.getDefault().language.toByteArray(Charset.forName("UTF-8"))

        val langSize = lang.size
        val contentSize = content.size

        val payload = ByteArrayOutputStream(1 + langSize + contentSize).apply {
            write((langSize and 0x1F))
            write(lang, 0, langSize)
            write(content, 0, contentSize)
        }

        val ndefRecord = NdefRecord(NdefRecord.TNF_WELL_KNOWN, NdefRecord.RTD_TEXT,
                ByteArray(0),
                payload.toByteArray())
        val nfcMessage = NdefMessage(arrayOf(ndefRecord))

        val nDef = Ndef.get(tag)

        nDef?.let {
            nDef.connect()
            if (nDef.maxSize < nfcMessage.toByteArray().size) {
                //Message too large to write to NFC tag
                return false
            }

            return if(nDef.isWritable) {
                nDef.writeNdefMessage(nfcMessage)
                nDef.close()
                //Message is written to tag
                true
            } else {
                false
            }
        }

        val nDefFormatableTag = NdefFormatable.get(tag)

        nDefFormatableTag.apply {
            return try {
                connect()
                format(nfcMessage)
                close()
                //The data is written to the tag
                true
            } catch (e: IOException) {
                Log.d(NDEF_TAG, "Ndef not formatted or not supported")
                false
            }
        }
    }

    override fun onPostExecute(result: Boolean) = mContext.messageWritten(result)

    companion object {
        const val NDEF_TAG = "NDEF"
    }

}
